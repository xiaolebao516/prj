"""Prespecified joint A/B and early-window study. Subject references are unknown; no pooled accuracy or stability claim.

Run python docs/research/g-range-onset-20260918/joint_candidates.py
Does not modify product behavior or inspect patient data.
"""
import candidates as core
import json, math, copy
import numpy as np

NAMES=('joint_peak','joint_short_peak','short_B_plateau')
ORIGINAL_REFINE=core.wave.refine

def refine_for(mode):
    def refine(e,l,onset,rough,low,high,post=120):
        width=60 if mode in ('joint_short_peak','short_B_plateau') else post
        # Keep the exact original search limits; only the matching window and/or
        # peak-vs-platform selection changes. No target velocity enters here.
        lag,c,detail=ORIGINAL_REFINE(e,l,onset,rough,low,high,post=width)
        if mode=='short_B_plateau' or 'peak_lag' not in detail:return lag,c,detail
        return detail['peak_lag'],detail['peak_corr'],detail
    return refine

def candidate_row(row,w,picks,c,name):
    assert core.wave.refine is ORIGINAL_REFINE
    if name=='short_B_plateau':
        r={'sequence':row['sequence'],'elapsed_ms':row['elapsed_ms'],'picks':picks}
        if any(picks[ch] and picks[ch]['onset']-picks[ch]['hit']>40 for ch in ('BD','BC')):return r
        scale=c['probe_distance_m']/c['sample_period_s']
        core.wave.refine=refine_for(name)
        try:b=core.bpair(w,picks,scale,'unchanged_onsets')
        finally:core.wave.refine=ORIGINAL_REFINE
        if not b or abs(b['lag']-b['rough_lag'])>70:return r
        # A, including its envelope fallback, keeps its original windows.
        a,_=core.wave.apair(w['AD'],w['AC'],picks['AD'],picks['AC'],b,scale)
        if not a or abs(a['lag']-b['lag'])>20:return r
        r.update(B=b,A=a,D=a['lag']-b['lag'],G=.5*(b['early_feature']+b['late_feature']-a['early_feature']-a['late_feature']),sos_patient=b['sos'],gates={})
        return r
    core.wave.refine=refine_for(name)
    try:return core.row_for(row,w,picks,c,'unchanged_onsets')
    finally:core.wave.refine=ORIGINAL_REFINE

def main():
    prior=json.loads((core.HERE/'candidates-results.json').read_text(encoding='utf-8'))
    assert all(core.sha(core.ROOT/s)==h for s,h in prior['source_hashes'].items())
    groups={};new_sessions=[];session=[]
    for m in prior['manifest']:
        path=core.ROOT/m['path'];assert core.sha(path)==m['sha256']
        rows=[json.loads(l) for l in path.read_bytes().splitlines() if l.strip()]
        cfg=rows[0]['config'];name='new' if cfg.get('B_onset_forward_limit') else path.name[6:14]+'-old'
        group=groups.setdefault(name,{'baseline':[],'candidates':{n:[] for n in NAMES},'frames':0})
        frames=[];alt={n:[] for n in NAMES}
        for row in rows:
            if row['event']!='frame':continue
            group['frames']+=1
            w={ch:core.wave.filtered(row,ch) for ch in ('BD','BC','AD','AC')}
            picks={ch:core.wave.arrival(x) for ch,x in w.items()}
            r=core.wave.clean(row);r['picks']=picks;frames.append(r)
            for n in NAMES:alt[n].append(candidate_row(row,w,picks,cfg,n))
        f=dict(file=path.name,config=cfg,frames=frames,events=[r for r in rows if r['event']!='frame'])
        core.study.replay(f,verify=True)
        b=core.study.replay(f,force_guard=True);group['baseline'].append(b)
        cases={n:core.study.replay(dict(f,frames=alt[n]),force_guard=True) for n in NAMES}
        for n,c in cases.items():group['candidates'][n].append(c)
        if name=='new':
            if b['summary']:session.append((b,cases))
            final=next((r for r in rows if r['event']=='final_round_selection'),None)
            if final:
                assert len(session)==5 and final['selected_indices']==list(range(5))
                original=[x[0]['summary']['sos'] for x in session]
                assert abs(np.mean(original)-final['final_sos'])<1e-8
                totals={}
                for n in NAMES:
                    c=[x[1][n] for x in session]
                    ss=[x['summary']['sos'] for x in c if x['summary'] and x['summary']['quality_pass']]
                    totals[n]=dict(retained=len(ss),conditional_final=sum(ss)/5 if len(ss)==5 else None,rounds=ss)
                new_sessions.append(dict(file=path.name,baseline=final['final_sos'],candidates=totals));session=[]
        print('joint replayed',path.name,flush=True)
    synthetic={}
    for n in NAMES:
        rows=[];fn=refine_for(n)
        t=np.arange(2000);e=np.exp(-.5*((t-780)/55)**2)*np.sin(2*np.pi*(t-780)/50)
        for true in (100,110,128,135,160,200,250):
            for err in (-2,0,2):
                l=np.zeros(2000);l[true:]=e[:-true]
                lag,_,_=fn(e,l,650,true+err,93,278)
                rows.append(dict(true=true,rough_error=err,estimated=lag,error=lag-true))
        synthetic[n]=rows
    output=dict(scope='No new acquisitions. Same101 logs and original per-file thresholds, onset40 protection. Joint modes apply same peak policy to A and B; short mode uses pre20/post60 instead of default B post120 and A post90. Temporal stability and D/G gates unchanged. No fitted offset or target-SOS selection.',
                manifest=prior['manifest'],source_hashes=prior['source_hashes'],synthetic=synthetic,groups={},conditional_new_sessions=new_sessions,
                subject_reference_status='Mixed participants; record-to-subject/reference mapping unavailable. Do not compute pooled accuracy or between-session stability.',candidate_session_retention={})
    for name,g in groups.items():
        output['groups'][name]=dict(files=len(g['baseline']),frames=g['frames'],baseline=core.study.summarize(g['baseline'],g['baseline']),
                candidates={n:core.study.summarize(c,g['baseline']) for n,c in g['candidates'].items()})
    for n in NAMES:
        v=[s['candidates'][n]['conditional_final'] for s in new_sessions]
        output['candidate_session_retention'][n]=dict(complete_sessions=sum(x is not None for x in v))
    assert all(core.sha(core.ROOT/m['path'])==m['sha256'] for m in prior['manifest'])
    assert all(core.sha(core.ROOT/s)==h for s,h in prior['source_hashes'].items())
    (core.HERE/'joint-results.json').write_text(json.dumps(output,ensure_ascii=False,indent=2),encoding='utf-8')
    print('DONE: joint candidates; all input/source hashes preserved',flush=True)

if __name__=='__main__':main()
