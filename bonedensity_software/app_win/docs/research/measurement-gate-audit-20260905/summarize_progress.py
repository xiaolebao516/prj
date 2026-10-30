"""Validate progress traces and compare against current dual G6, not obsolete profiles."""
import json
from pathlib import Path
import statistics as st
import sys

source = Path(sys.argv[1]).resolve()
report = json.loads(source.read_text(encoding='utf-8'))
variants = ('original_G6', 'original_G8', 'original_G10', 'corrected_G6', 'corrected_G10')
out = []
for file in report['files']:
    assert file['G10_raw_candidate_matches'] and file['G8_raw_candidate_matches']
    baseline = file['original_G6']
    base_trace = baseline['accepted_trace_including_discarded']
    base_seq = {x['sequence'] for x in base_trace}
    base_kept = {x['sequence'] for x in base_trace[-30:]}
    entry = {'file': file['file'], 'variants': {}}
    for key in variants:
        r = file[key]
        trace = r['accepted_trace_including_discarded']
        assert r['reached_30'] and r['round_summary']['quality_pass']
        assert len(trace) == 30 + r['discarded_values']
        assert trace[0]['elapsed_ms'] == r['first_accepted_ms']
        kept = trace[-30:]
        trimmed = st.mean(sorted(x['sos'] for x in kept)[6:-6])
        assert abs(trimmed - r['round_summary']['sos']) < 1e-8
        additional = [x['sos'] for x in trace if x['sequence'] not in base_seq]
        newly_kept = [x['sos'] for x in kept if x['sequence'] not in base_kept]
        stats = lambda values: dict(n=len(values), min=min(values), max=max(values)) if values else dict(n=0)
        gaps = [b['elapsed_ms']-a['elapsed_ms'] for a,b in zip(trace,trace[1:])]
        entry['variants'][key] = dict(first_s=r['first_accepted_ms']/1000,
            first_retained_s=kept[0]['elapsed_ms']/1000,
            retained_tenth_s=kept[9]['elapsed_ms']/1000,
            completion_s=r['completion_ms_on_recorded_timeline']/1000,
            max_gap_after_first_s=max(gaps, default=0)/1000,
            discarded=r['discarded_values'], sos=r['round_summary']['sos'],
            sos_delta=r['round_summary']['sos']-baseline['round_summary']['sos'],
            additional_accepted_including_discarded=stats(additional),
            newly_retained=stats(newly_kept), retained=stats([x['sos'] for x in kept]))
    out.append(entry)
summary = dict(files=out, scope='All19 raw G8 and G10 candidates match actual product replay. Accepted traces include discards; last30 reconstruct the actual completed round. Reference3800-3900 never filters samples.')
for group, start, end in [('new_session1',0,5), ('new_session2',5,10), ('old_eight',10,18), ('old_failed',18,19)]:
    summary[group] = {}
    for key in variants:
        rows = [x['variants'][key] for x in out[start:end]]
        summary[group][key] = dict(active_s=sum(x['completion_s'] for x in rows),
            discarded=sum(x['discarded'] for x in rows),
            round_min=min(x['sos'] for x in rows), round_max=max(x['sos'] for x in rows))
dest = Path(sys.argv[2]).resolve()
assert dest != source
dest.write_text(json.dumps(summary, ensure_ascii=False, indent=2)+'\n', encoding='utf-8')
print(json.dumps({k:v for k,v in summary.items() if k != 'files'}, ensure_ascii=False))
