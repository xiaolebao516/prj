"""Verify B audit inputs and summarize fixed retained-frame diagnostics, not replay."""
import hashlib
import json
from pathlib import Path
import statistics as st
import sys

audit_path = Path(sys.argv[1]).resolve()
audit = json.loads(audit_path.read_text(encoding='utf-8'))
audit_cwd = Path(sys.argv[2]).resolve()
windows = ('full', 'front', 'tail', 'long', 'onset_minus10', 'onset_plus10',
           'extended15', 'physical')
reports = []
for entry in audit['files']:
    source = (audit_cwd / entry['source']).resolve()
    raw = source.read_bytes()
    assert hashlib.sha256(raw).hexdigest() == entry['sha256']
    rows = [json.loads(line) for line in raw.splitlines() if line.strip()]
    kept = []
    for row in rows:
        if row['event'] == 'discard_partial':
            assert len(kept) == row['discarded_values']
            kept = []
        elif row['event'] == 'frame' and row['decision'] == 'accepted':
            kept.append(row)
    frames = [f for f in entry['frames'] if f['kept']]
    assert [f['sequence'] for f in frames] == [f['sequence'] for f in kept]
    assert len(entry['frames']) + entry['invalid_B'] == sum(r['event'] == 'frame' for r in rows)
    summary = dict(file=entry['file'], sha256=entry['sha256'],
                   valid_B=len(entry['frames']), invalid_B=entry['invalid_B'], kept=len(frames))
    if frames:
        assert len(frames) == 30
        trimmed = lambda xs: st.mean(sorted(xs)[6:-6])
        baseline = trimmed([f['B']['sos'] for f in frames])
        round_row, = [r for r in rows if r['event'] == 'round_summary']
        assert abs(baseline - round_row['sos']) < 1e-8
        scale = entry['config']['probe_distance_m'] / entry['config']['sample_period_s']
        summary.update(original_round=baseline, original_quality_pass=round_row['quality_pass'],
                       selected_minus_peak_lag_mean=st.mean(f['B']['lag']-f['full']['peak_lag'] for f in frames),
                       disconnected_plateaus=sum(f['plateau_components'] > 1 for f in frames),
                       windows={}, rough_hint_sensitivity={})
        for window in windows:
            summary['windows'][window] = dict(
                fixed_frame_trimmed_sos=trimmed([scale/f[window]['peak_lag'] for f in frames]),
                mean_correlation=st.mean(f[window]['peak_corr'] for f in frames),
                at_search_edge=sum(f[window]['edge'] for f in frames))
        for key in ('rough_minus5', 'rough_plus5'):
            summary['rough_hint_sensitivity'][key] = dict(
                fixed_frame_trimmed_sos=trimmed([scale/f[key]['lag'] for f in frames]),
                below_B_floor=sum(f[key]['corr'] < .55 for f in frames))
        summary['rough_hint_sensitivity']['unchanged_both'] = sum(
            f['rough_minus5']['lag'] == f['B']['lag'] == f['rough_plus5']['lag'] for f in frames)
    reports.append(summary)
assert len(reports) == 19
result = dict(scope='Fixed originally retained frames only; alternate estimates do not rerun A, D/G, gates, stability or round acceptance. Not candidate completion/accuracy evidence.',
              valid_B=sum(f['valid_B'] for f in reports), invalid_B=sum(f['invalid_B'] for f in reports), files=reports)
for session in (0, 1):
    group = reports[session*5:session*5+5]
    result[f'session{session+1}_ranges'] = {'original': max(f['original_round'] for f in group)-min(f['original_round'] for f in group)}
    for window in windows:
        values = [f['windows'][window]['fixed_frame_trimmed_sos'] for f in group]
        result[f'session{session+1}_ranges'][window] = max(values)-min(values)
output = Path(sys.argv[3]).resolve()
assert output != audit_path and output not in [(audit_cwd/f['source']).resolve() for f in audit['files']]
output.write_text(json.dumps(result, ensure_ascii=False, indent=2)+'\n', encoding='utf-8')
print(json.dumps({k:v for k,v in result.items() if k != 'files'}, ensure_ascii=False))
