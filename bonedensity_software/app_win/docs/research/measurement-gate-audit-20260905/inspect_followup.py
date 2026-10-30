"""Read-only log analysis; stdout JSON. No medical/reference labels inferred."""
import collections
import hashlib
import json
import pathlib
import statistics as st
import sys

def stats(values):
    return dict(n=len(values), min=min(values), max=max(values), mean=st.mean(values),
                sd=st.pstdev(values)) if values else dict(n=0)

def block(frames):
    result={k:stats([f[k] for f in frames]) for k in ('sos_patient','D','G')}
    result['A_corr']=stats([f['A']['corr'] for f in frames])
    result['B_corr']=stats([f['B']['corr'] for f in frames])
    result['B_lags']=dict(collections.Counter(f['B']['lag'] for f in frames))
    for k in ('lag','rough_lag','early_feature','late_feature'):
        result['B_'+k]=stats([f['B'][k] for f in frames])
    result['original_A_fails']=sum(f.get('A_quality_trial',{}).get('original_corr',f['A']['corr'])<.78 for f in frames)
    if frames:
        result['start_ms'],result['end_ms']=frames[0]['elapsed_ms'],frames[-1]['elapsed_ms']
    return result

results=[]
for file in sorted(pathlib.Path(sys.argv[1]).glob('round-*.jsonl')):
    data=file.read_bytes();rows=[json.loads(x) for x in data.splitlines() if x.strip()]
    assert rows[0]['event']=='start' and rows[-1]['event']=='stop'
    assert [r['sequence'] for r in rows]==list(range(1,len(rows)+1))
    assert all(b['elapsed_ms']>=a['elapsed_ms'] for a,b in zip(rows,rows[1:]))
    kept=[];discarded=[];summaries=[]
    for row in rows:
        if row['event']=='discard_partial':
            assert row['discarded_values']==len(kept)
            discarded+=kept;kept=[]
        if row['event']=='frame' and row['decision']=='accepted': kept.append(row)
        if row['event']=='round_summary': summaries.append(row)
    assert len(kept)==30 and len(summaries)==1
    values=sorted(f['sos_patient'] for f in kept)
    assert abs(st.mean(values[6:-6])-summaries[0]['sos'])<1e-8
    frames=[r for r in rows if r['event']=='frame']
    first=next(r['elapsed_ms'] for r in frames if r['decision']=='accepted')
    g_only=[];runs=[];run=[]
    for f in frames:
        g=f.get('gates',{})
        only=bool(g) and not g['G'] and all(g[k] for k in ('B_jump','boundary','AB_diff','direction','corr_A','corr_B','D'))
        if only: g_only.append(f);run.append(f)
        elif run: runs.append(run);run=[]
    if run:runs.append(run)
    post=[r for r in frames if r['elapsed_ms']>=first]
    accepted=[r for r in post if r['decision']=='accepted']
    result=dict(file=file.name,sha256=hashlib.sha256(data).hexdigest(),config=rows[0]['config'],frames=len(frames),
        first_accepted_ms=first,summary=summaries[0],kept=block(kept),discarded=block(discarded),
        first10=block(kept[:10]),last10=block(kept[-10:]),
        accepted_trace=[dict(ms=f['elapsed_ms'],sos=f['sos_patient'],lag=f['B']['lag'],rough=f['B']['rough_lag'],
                             D=f['D'],G=f['G'],corrA=f['A']['corr'],corrB=f['B']['corr']) for f in kept],
        post_first_frames=len(post),post_first_accepted=len(accepted),
        max_gap_after_first_ms=max((b['elapsed_ms']-a['elapsed_ms'] for a,b in zip(accepted,accepted[1:])),default=0),
        g_only_before=block([f for f in g_only if f['elapsed_ms']<first]),
        g_only_after=block([f for f in g_only if f['elapsed_ms']>=first]),
        longest_g_only_runs=[block(r) for r in sorted(runs,key=len,reverse=True)[:3]])
    results.append(result)
assert len(results)==10
print(json.dumps(dict(files=results),ensure_ascii=False,indent=2))
