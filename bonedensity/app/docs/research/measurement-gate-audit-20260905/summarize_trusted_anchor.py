"""Read-only full-flow comparison; no identity inference or accuracy labels."""
import json
import pathlib
import statistics as st
import sys
from summarize_targeted_wave import best_cluster


def exact_five(values):
    if len(values) < 5:
        return None
    ordered = sorted(values)
    windows = [ordered[i:i+5] for i in range(len(ordered)-4)]
    return st.mean(min(windows, key=lambda x: (x[-1]-x[0], sum(abs(v-st.median(x)) for v in x))))


def main(replay_path, triage_path):
    files = json.loads(pathlib.Path(replay_path).read_text(encoding='utf-8'))['files']
    triage = {x['file']: x for x in json.loads(pathlib.Path(triage_path).read_text(encoding='utf-8'))['files']}
    changed = []
    finals = []
    pools = {'baseline': [], 'candidate': []}
    tainted = False
    first_rounds = []
    for f in files:
        t = triage[f['file']]
        assert f['sha256'] == t['sha256']
        if t['round'] == 1:
            pools = {'baseline': [], 'candidate': []}
            tainted = False
            first_rounds.append(f)
        tainted |= f['damaged_prefix_only']
        b, c = f['baseline'], f['candidate']
        entry = {'file': f['file'], 'damaged_prefix_only': f['damaged_prefix_only']}
        for key in ('first_accepted_ms', 'completion_ms_on_recorded_timeline', 'reached_30', 'discarded_values'):
            if b[key] != c[key]:
                entry[key] = [b[key], c[key]]
        for label, r in [('baseline', b), ('candidate', c)]:
            if r['reached_30']:
                kept = r['accepted_trace_including_discarded'][-30:]
                assert len(kept) == 30
                values = sorted(v['sos'] for v in kept)
                assert abs(st.mean(values[6:-6])-r['round_summary']['sos']) < 1e-7
                entry[label+'_first_retained_ms'] = kept[0]['elapsed_ms']
                entry[label+'_round_sos'] = r['round_summary']['sos']
                if r['round_summary']['quality_pass']:
                    pools[label].append(r['round_summary']['sos'])
        if f['changes'] or len(entry) > 2 and b != c:
            entry['feature_changes'] = len(f['changes'])
            entry['newly_accepted_sos_including_discarded'] = c['newly_accepted_sos']
            changed.append(entry)
        # Do not insert recovered suffix summaries into the candidate replay.
        if t['clusters'] and len(t['clusters'][-1]['accepted_sos']) >= 5:
            item = {'file': f['file'], 'incomplete_sequence': tainted}
            if not tainted:
                original = best_cluster(pools['baseline'], 180)
                candidate = best_cluster(pools['candidate'], 180)
                assert len(original) == len(t['clusters'][-1]['accepted_sos'])
                assert all(abs(x-y) < 1e-7 for x,y in zip(original,t['clusters'][-1]['accepted_sos']))
                item.update(baseline_existing_final=st.mean(original),
                            baseline_exact_five=exact_five(original),
                            candidate_existing_final=st.mean(candidate),
                            candidate_exact_five=exact_five(candidate))
            finals.append(item)
    completed = [f for f in files if f['baseline']['reached_30'] and f['candidate']['reached_30']]
    gains = [f['baseline']['completion_ms_on_recorded_timeline']-f['candidate']['completion_ms_on_recorded_timeline'] for f in completed]
    deltas = [f['candidate']['round_summary']['sos']-f['baseline']['round_summary']['sos'] for f in completed]
    longest = max((f for f in first_rounds if f['baseline']['reached_30']), key=lambda f:f['baseline']['completion_ms_on_recorded_timeline'])
    return dict(scope='Validated recorded-trajectory counterfactual; not prospective speed, participant comparison, or accuracy.',
                logs=len(files), damaged_prefixes=sum(f['damaged_prefix_only'] for f in files),
                verified_gates=sum(f['gate_frames_verified'] for f in files),
                feature_changes=sum(len(f['changes']) for f in files),
                restored_prechecks=sum(sum(c['new_prechecks'] for c in f['changes']) for f in files),
                completed={k:sum(f[k]['reached_30'] for f in files) for k in pools},
                discarded={k:sum(f[k]['discarded_values'] for f in files) for k in pools},
                earlier_completions=sum(g>0 for g in gains), later_completions=sum(g<0 for g in gains),
                completion_gain_ms_nonzero=[g for g in gains if g],
                max_abs_round_sos_delta=max(map(abs,deltas)),
                longest_first_round={k:longest[k]['completion_ms_on_recorded_timeline'] for k in pools},
                changes=changed, conditional_final_sequences=finals)


if __name__ == '__main__':
    print(json.dumps(main(sys.argv[1],sys.argv[2]),ensure_ascii=False,indent=2))
