"""Read-only streaming triage of anonymous experiment logs; JSON to stdout.

No participant inference, threshold changes, or input repairs. NUL tails are
reported separately; their valid prefix remains usable only when validated.
"""
import collections
import datetime
import hashlib
import json
import pathlib
import statistics
import sys


def inspect(path):
    out = dict(file=path.name, bytes=path.stat().st_size)
    events = collections.Counter()
    decisions = collections.Counter()
    pre_decisions = collections.Counter()
    pre_failures = collections.Counter()
    pre_only = collections.Counter()
    all_failures = collections.Counter()
    all_only = collections.Counter()
    post_decisions = collections.Counter()
    sole_g_values = []
    sole_d_values = collections.Counter()
    kept = []
    summaries = []
    clusters = []
    errors = []
    first = None
    discarded = 0
    frames = 0
    last_ms = 0
    sequence = 0
    digest = hashlib.sha256()
    tail = 0
    nonzero_tail = 0
    suffix_events = []
    offset = 0
    with path.open('rb') as stream:
        while True:
            line = stream.readline(1024 * 1024)
            if not line:
                break
            digest.update(line)
            if b'\x00' in line or not line.endswith(b'\n'):
                out['tail_offset'] = offset
                tail += len(line)
                nonzero_tail += len(line) - line.count(b'\x00')
                # Salvage only independently complete metadata lines, never
                # join damaged waveform fragments or claim sequence integrity.
                def salvage(chunk):
                    if not chunk.strip(b'\x00'):
                        return
                    for fragment in chunk.split(b'\n'):
                        if not fragment.startswith(b'{') or not fragment.endswith(b'}') or b'\x00' in fragment:
                            continue
                        try:
                            record = json.loads(fragment)
                        except ValueError:
                            continue
                        if record.get('event') in ('round_summary', 'round_cluster', 'stop'):
                            suffix_events.append(record)
                salvage(line)
                while True:
                    chunk = stream.read(1024 * 1024)
                    if not chunk:
                        break
                    digest.update(chunk)
                    tail += len(chunk)
                    nonzero_tail += len(chunk) - chunk.count(b'\x00')
                    salvage(chunk)
                break
            offset += len(line)
            try:
                row = json.loads(line)
            except (ValueError, UnicodeError):
                errors.append(['invalid_json', offset - len(line)])
                continue
            if row['sequence'] != sequence + 1:
                errors.append(['sequence', row['sequence'], sequence])
            if row['elapsed_ms'] < last_ms:
                errors.append(['elapsed', row['sequence']])
            sequence = row['sequence']
            last_ms = row['elapsed_ms']
            event = row['event']
            events[event] += 1
            if event == 'start':
                out['config'] = row['config']
                out['utc'] = row['utc']
                out['local'] = datetime.datetime.fromisoformat(row['utc'].replace('Z', '+00:00')).astimezone(datetime.timezone(datetime.timedelta(hours=8))).isoformat()
            elif event == 'frame':
                frames += 1
                decisions[row['decision']] += 1
                gates = row.get('gates', {})
                failed = [k for k in ('B_jump', 'boundary', 'AB_diff', 'direction',
                                      'corr_A', 'corr_B', 'D', 'G') if k in gates and not gates[k]]
                all_failures.update(failed)
                if len(failed) == 1:
                    all_only.update(failed)
                if failed == ['G']:
                    sole_g_values.append(abs(row['G']))
                if failed == ['D']:
                    sole_d_values[row['D']] += 1
                if first is None:
                    pre_decisions[row['decision']] += 1
                    pre_failures.update(failed)
                    if len(failed) == 1:
                        pre_only.update(failed)
                else:
                    post_decisions[row['decision']] += 1
                if row['decision'] == 'accepted':
                    if first is None:
                        first = last_ms
                    kept.append(dict(ms=last_ms, sos=row['sos_patient'], lag=row['B']['lag'],
                                     G=row['G'], D=row['D'], A=row['A']['corr'], B=row['B']['corr']))
            elif event == 'discard_partial':
                if row['discarded_values'] != len(kept):
                    errors.append(['discard_count', row['discarded_values'], len(kept)])
                discarded += len(kept)
                kept = []
            elif event == 'round_summary':
                summaries.append(row)
                values = sorted(k['sos'] for k in kept)
                if len(values) != 30:
                    errors.append(['summary_count', len(values)])
                elif abs(statistics.mean(values[6:-6]) - row['sos']) > 1e-7:
                    errors.append(['summary_sos'])
            elif event == 'round_cluster':
                clusters.append(row)
    out.update(sha256=digest.hexdigest(), frames=frames, events=dict(events), decisions=dict(decisions),
               pre_decisions=dict(pre_decisions), pre_failures=dict(pre_failures), pre_only=dict(pre_only),
               all_failures=dict(all_failures), all_only=dict(all_only), post_decisions=dict(post_decisions),
               first_ms=first, last_ms=last_ms, discarded=discarded, kept=kept,
               summaries=summaries, clusters=clusters, errors=errors,
               tail_bytes=tail, tail_nonzero_bytes=nonzero_tail,
               unverified_suffix_events=suffix_events)
    out['kept'] = dict(count=len(kept), first_ms=kept[0]['ms'] if kept else None,
                       sos_min=min((k['sos'] for k in kept), default=None),
                       sos_max=max((k['sos'] for k in kept), default=None))
    out['sole_G_distribution'] = dict(n=len(sole_g_values),
        abs_median=statistics.median(sole_g_values) if sole_g_values else None,
        abs_le8=sum(g <= 8 for g in sole_g_values), abs_gt20=sum(g > 20 for g in sole_g_values))
    out['sole_D_values'] = dict(sole_d_values)
    return out


if __name__ == '__main__':
    results = []
    for path in sorted(pathlib.Path(sys.argv[1]).glob('round-*.jsonl')):
        results.append(inspect(path))
    print(json.dumps(dict(files=results), ensure_ascii=False))
