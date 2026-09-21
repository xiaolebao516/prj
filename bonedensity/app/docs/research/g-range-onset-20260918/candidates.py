"""Full feature/state offline candidates. Never edits product or input records.

python docs/research/g-range-onset-20260918/candidates.py
Four fixed candidates; no tuning to a desired SOS. Original guard retained.
"""
from pathlib import Path
import importlib.util, sys, json, hashlib, math, copy
import numpy as np
ROOT=Path(__file__).resolve().parents[3]
HERE=Path(__file__).resolve().parent
sys.path.insert(0,str(ROOT/'docs/research/measurement-wave-audit-20260915'))
import analyze as wave
spec=importlib.util.spec_from_file_location('gstudy',HERE/'analyze.py')
study=importlib.util.module_from_spec(spec);spec.loader.exec_module(study)
NAMES=('first_hit','ratio30','edge5','local_peak')

def sha(p): return hashlib.sha256(p.read_bytes()).hexdigest()

def bpair(w,picks,scale,name):
    pp={ch:copy.copy(picks[ch]) for ch in ('BD','BC')}
    if any(p is None for p in pp.values()): return None
    if name=='first_hit':
        for p in pp.values():p['onset']=p['hit']
    elif name=='ratio30':
        pp={ch:wave.arrival(w[ch],.3) for ch in pp}
    if any(p is None or p['onset']-p['hit']>40 for p in pp.values()):return None
    b,detail=wave.pair(w['BD'],w['BC'],pp['BD'],pp['BC'],scale)
    if name not in ('edge5','local_peak') or not b:return b
    lo,hi=detail['low'],detail['high']
    if name=='edge5' and detail.get('edge'):
        if detail['peak_lag']==lo:lo=max(max(1,math.floor(scale/5000)-5),lo-5)
        if detail['peak_lag']==hi:hi=min(math.ceil(scale/1800)+5,hi+5)
    lags=np.arange(lo,hi+1);cs=wave.correlations(w['BD'],w['BC'],b['early_feature'],lags)
    p=int(np.argmax(cs));j=p
    if name=='edge5':
        alpha=min(1,max(0,(int(lags[p])-106)/99))
        band=np.flatnonzero((cs>0)&(cs>=cs[p]*(.92+alpha*.03)))
        if len(band):j=int(band[int(math.floor(alpha*(len(band)-1)+.5))])
    if cs[j]<.25:return None
    return dict(b,lag=int(lags[j]),corr=float(cs[j]),sos=scale/int(lags[j]))

def row_for(row,w,picks,c,name):
    r={'sequence':row['sequence'],'elapsed_ms':row['elapsed_ms'],'picks':picks}
    if any(picks[ch] and picks[ch]['onset']-picks[ch]['hit']>40 for ch in ('BD','BC')):return r
    scale=c['probe_distance_m']/c['sample_period_s']
    b=bpair(w,picks,scale,name)
    if not b or abs(b['lag']-b['rough_lag'])>70:return r
    a,_=wave.apair(w['AD'],w['AC'],picks['AD'],picks['AC'],b,scale)
    if not a or abs(a['lag']-b['lag'])>20:return r
    r.update(B=b,A=a,D=a['lag']-b['lag'],G=.5*(b['early_feature']+b['late_feature']-a['early_feature']-a['late_feature']),sos_patient=b['sos'],gates={})
    return r

def synthetic():
    out=[]
    t=np.arange(2000);early=np.exp(-.5*((t-780)/55)**2)*np.sin(2*np.pi*(t-780)/50)
    for true in (100,110,128,135,160,200,250):
        for rough_error in (-2,0,2):
            late=np.zeros(2000);late[true:]=early[:-true]
            lag,c,detail=wave.refine(early,late,650,true+rough_error,93,278)
            lags=np.arange(detail['low'],detail['high']+1)
            cs=wave.correlations(early,late,650,lags);peak=int(lags[np.argmax(cs)])
            assert peak==true
            out.append(dict(true_lag=true,rough_error=rough_error,current_lag=lag,current_error=lag-true,local_peak=peak,corr=c))
    return out

def main():
    previous=json.loads((HERE/'results.json').read_text(encoding='utf-8'))
    manifest=previous['manifest'];groups={};examples=[]
    source_hashes={s:sha(ROOT/s) for s in study.SOURCES}
    for m in manifest:
        path=ROOT/m['path'];assert sha(path)==m['sha256']
        rows=[json.loads(l) for l in path.read_bytes().splitlines() if l.strip()]
        c=rows[0]['config'];assert c['B_only'] and c['SOS_offset']==0 and c['B_clipped_peak_extension']==0
        name='new' if c.get('B_onset_forward_limit') else path.name[6:14]+'-old'
        group=groups.setdefault(name,{'original':[],'baseline':[],'candidates':{n:[] for n in NAMES},'frames':0})
        frames=[];alts={n:[] for n in NAMES};scale=c['probe_distance_m']/c['sample_period_s']
        for row in rows:
            if row['event']!='frame':continue
            group['frames']+=1
            w={ch:wave.filtered(row,ch) for ch in ('BD','BC','AD','AC')}
            picks={ch:wave.arrival(x) for ch,x in w.items()}
            r=wave.clean(row);r['picks']=picks;frames.append(r)
            # Verify real features before constructing any candidate, including
            # old records whose guard was not yet active.
            if 'B' in row:
                b,_=wave.pair(w['BD'],w['BC'],picks['BD'],picks['BC'],scale)
                wave.verify(b,row['B'],(path.name,row['sequence'],'B'))
                if 'A' in row:
                    a,_=wave.apair(w['AD'],w['AC'],picks['AD'],picks['AC'],b,scale)
                    wave.verify(a,row['A'],(path.name,row['sequence'],'A'))
            for n in NAMES:alts[n].append(row_for(row,w,picks,c,n))
        f=dict(file=path.name,config=c,frames=frames,events=[r for r in rows if r['event']!='frame'])
        group['original'].append(study.replay(f,verify=True))
        base=study.replay(f,force_guard=True);group['baseline'].append(base)
        for n in NAMES:
            case=study.replay(dict(f,frames=alts[n]),force_guard=True)
            group['candidates'][n].append(case)
            if name=='new' and base['summary'] and case['summary']:
                d=case['summary']['sos']-base['summary']['sos']
                if abs(d)>100:examples.append(dict(file=path.name,candidate=n,delta=d,before=base['summary'],after=case['summary']))
        print('verified and replayed',path.name,flush=True)
    out=dict(scope='Raw feature reconstruction plus Python state replay; no Qt/device execution. Original per-file G/A kept; onset40 applied to baseline and all candidates. Original and changed onsets must satisfy forward guard. Stop at original recording endpoint. No pose labels or reference SOS assumed.',
             manifest=manifest,source_hashes=source_hashes,synthetic=synthetic(),groups={},large_new_shifts=examples)
    for name,g in groups.items():
        out['groups'][name]=dict(frames=g['frames'],files=len(g['baseline']),original=study.summarize(g['original'],g['original']),baseline=study.summarize(g['baseline'],g['baseline']),candidates={n:study.summarize(cases,g['baseline']) for n,cases in g['candidates'].items()})
    assert all(sha(ROOT/m['path'])==m['sha256'] for m in manifest)
    assert all(sha(ROOT/s)==h for s,h in source_hashes.items())
    (HERE/'candidates-results.json').write_text(json.dumps(out,ensure_ascii=False,indent=2),encoding='utf-8')
    print('DONE: input/source preserved; baselines matched; candidates evaluated',flush=True)

if __name__=='__main__':main()
