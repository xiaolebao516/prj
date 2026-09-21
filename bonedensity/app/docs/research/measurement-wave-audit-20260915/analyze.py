"""Offline diagnostic replica; never opens the device or writes input logs.

Run from project root: python docs/research/measurement-wave-audit-20260915/analyze.py
Validates raw B/A feature reconstruction against every September 15 log frame.
This is not a replay of the complete Qt acquisition/round state machine.
"""
from pathlib import Path
import base64
import collections
import hashlib
import json
import math
import sys
import numpy as np
from scipy.signal import find_peaks

ROOT = Path(__file__).resolve().parents[3]
OUT = ROOT / 'build/wave-audit-20260915'
INPUT = ROOT / 'build/Desktop_Qt_6_5_3_MinGW_64_bit_Debug/debug/measurement-experiments'

def i0(x):
    total = term = 1.0
    for k in range(1, 50):
        term *= x*x/4/(k*k)
        total += term
        if term < 1e-9*total:
            break
    return total

def fir():
    fl = 300000/62500000
    wc = 2*math.pi*1250000/62500000
    h = []
    for n in range(81):
        x = n-40
        v = 2*fl if not x else math.sin(2*math.pi*fl*x)/(math.pi*x)
        h.append(v*i0(5*math.sqrt(max(0,1-(x/40)**2)))/i0(5)*2*math.cos(wc*x))
    dc = sum(h)/len(h)
    h = [c-dc for c in h]
    mag = math.hypot(sum(c*math.cos(-wc*n) for n,c in enumerate(h)),
                     sum(c*math.sin(-wc*n) for n,c in enumerate(h)))
    return np.array(h)/mag

H = fir()

def filtered(row, channel):
    raw = np.frombuffer(base64.b64decode(row['raw_'+channel], validate=True), dtype='<u2').astype(float)
    assert len(raw) == 2000
    b0,b1,e0,e1,rend = (20,105,115,500,560) if channel[0]=='B' else (0,15,18,480,550)
    x = raw-np.sort(raw[b0:b1+1])[(b1-b0+1)//2]
    x[e0:e1+1] = 0
    t = np.arange(1,rend-e1+1)/(rend-e1)
    x[e1+1:rend+1] *= t*t*(3-2*t)
    return np.convolve(x,H)[:len(x)]

def envelope(x):
    return np.convolve(np.abs(x),np.ones(20))[:len(x)]/np.minimum(np.arange(1,len(x)+1),20)

def arrival(x, ratio=.2, k=4):
    env = envelope(x)
    mean = float(np.mean(env[560:616])); sigma = float(np.std(env[560:616]))
    threshold = mean+k*sigma
    hits = np.flatnonzero(np.convolve((env[620:1601]>threshold).astype(int),np.ones(8,dtype=int),'valid')==8)
    if not len(hits):
        return None
    hit = 620+int(hits[0])
    peak = hit+int(np.argmax(env[hit:min(1600,hit+140)+1]))
    level = mean+ratio*(env[peak]-mean)
    back = np.flatnonzero(env[620:peak+1]<=level)
    onset = 620+int(back[-1]) if len(back) else hit
    onset = max(onset,hit-40)
    return dict(onset=onset,hit=hit,peak=peak,peak_env=float(env[peak]),noise=mean,
                sigma=sigma,threshold=threshold,level=float(level))

def correlations(e,l,onset,lags,pre=20,post=120):
    lags = np.asarray(lags,dtype=int)
    idx = np.arange(max(0,onset-pre),min(len(e)-1,onset+post)+1)
    a = e[idx]; a = a-a.mean()
    b = l[idx[None,:]+lags[:,None]]; b = b-b.mean(axis=1)[:,None]
    den = np.sqrt(np.sum(a*a)*np.sum(b*b,axis=1))
    return np.divide(np.sum(b*a,axis=1),den,out=np.full(len(lags),-2.),where=den>=1e-12)

def refine(e,l,onset,rough,low,high,post=120):
    if low<=rough<=high:
        width = min(55,max(30,int(.25*abs(rough))))
        left,right = max(low,rough-width),min(high,rough+width)
    elif rough<low and rough>=low-35 and rough>=60:
        left,right = low,min(high,rough+55)
    else:
        left,right = low,high
    lags = np.arange(left,right+1); cs = correlations(e,l,onset,lags,post=post)
    p = int(np.argmax(cs))
    if cs[p]<=0:
        return rough,0,dict(low=left,high=right)
    alpha = min(1,max(0,(int(lags[p])-106)/99))
    band = np.flatnonzero((cs>0)&(cs>=cs[p]*(.92+alpha*.03)))
    chosen = int(band[int(math.floor(alpha*(len(band)-1)+.5))])
    return int(lags[chosen]),float(cs[chosen]),dict(low=left,high=right,peak_lag=int(lags[p]),
        peak_corr=float(cs[p]),edge=p in (0,len(lags)-1),components=int(1+np.sum(np.diff(band)>1)),
        band_low=int(lags[band[0]]),band_high=int(lags[band[-1]]))

def valley(x,start,end,depth,ratio):
    start=max(1,min(start,len(x)-2)); end=max(1,min(end,len(x)-2))
    raw=[i for i in range(start,end+1) if x[i]<x[i-1] and x[i]<=x[i+1] and -x[i]>=depth]
    if not raw:
        return None
    merged=[]; group=[raw[0]]
    for i in raw[1:]:
        if i-group[-1]<=20:
            group.append(i)
        else:
            merged.append(min(group,key=lambda k:x[k])); group=[i]
    merged.append(min(group,key=lambda k:x[k]))
    threshold=max(depth,ratio*max(-x[i] for i in merged))
    return next((i for i in merged if -x[i]>=threshold),None)

def pair(e,l,pe,pl,scale,forced=None):
    if pe is None or pl is None:
        return None,None
    lo=max(1,math.floor(scale/5000)-5); hi=max(lo+1,math.ceil(scale/1800)+5)
    if forced:
        lo=max(lo,forced[0]);hi=min(hi,forced[1])
    if lo>=hi:
        return None,None
    rough=pl['onset']-pe['onset']
    lag,c,detail=refine(e,l,pe['onset'],rough,lo,hi)
    if abs(lag)<lo or abs(lag)>hi or c<.25:
        return None,detail
    return dict(early_feature=pe['onset'],late_feature=pl['onset'],rough_lag=rough,lag=lag,corr=c,sos=scale/abs(lag)),detail

def apair(ad,ac,pa,pac,b,scale):
    a=None
    if pa is not None:
        e=valley(ad,620,1300,30,.30)
        if e is None:
            e=valley(ad,pa['onset']-60,pa['onset']+150,30,.30)
        late=valley(ac,e+b['lag']-20,e+b['lag']+20,25,.25) if e is not None else None
        if late is not None:
            rough=late-e; lag,c,detail=refine(ad,ac,e,rough,max(1,rough-15),rough+15,90)
            front=float(correlations(ad,ac,e,[lag],20,30)[0])
            mid=float(correlations(ad,ac,e,[lag],0,60)[0])
            a=dict(early_feature=e,late_feature=late,rough_lag=rough,lag=lag,corr=min(front,mid),sos=scale/abs(lag))
    if a is not None:
        return a,'valley'
    a,_=pair(ad,ac,pa,pac,scale,(max(1,b['lag']-20),b['lag']+20))
    return a,'envelope_fallback'

def verify(got,logged,where):
    assert (got is not None)==logged['valid'],(where,'valid',got,logged)
    if got is not None:
        for k,v in got.items():
            assert abs(v-logged[k])<1e-8,(where,k,v,logged[k])

def clean(row):
    return {k:v for k,v in row.items() if not k.startswith('raw_')}

def main():
    global OUT
    date=sys.argv[1] if len(sys.argv)>1 else '20260915'
    assert date in ('20260914','20260915')
    OUT=ROOT/('build/wave-audit-'+date)
    OUT.mkdir(parents=True, exist_ok=True)
    files=sorted(INPUT.glob('round-'+date+'*.jsonl'))
    assert files
    out=[]; manifest=[]; count=0; verifiedA=0; examples={}
    for file in files:
        data=file.read_bytes(); sha=hashlib.sha256(data).hexdigest()
        rows=[json.loads(s) for s in data.splitlines() if s.strip()]
        assert [r['sequence'] for r in rows]==list(range(1,len(rows)+1))
        assert rows[-1]['event']=='stop'
        cfg=rows[0]['config']; scale=cfg['probe_distance_m']/cfg['sample_period_s']
        assert cfg['implementation']=='production-relock-auto-next-20260908-v1' and cfg['B_clipped_peak_extension']==0
        fr=[]
        for row in rows:
            if row['event']!='frame':
                continue
            count+=1
            w={ch:filtered(row,ch) for ch in ['BD','BC','AD','AC']}
            picks={ch:arrival(x) for ch,x in w.items()}
            b,detail=pair(w['BD'],w['BC'],picks['BD'],picks['BC'],scale)
            verify(b,row['B'],(file.name,row['sequence'],'B'))
            if 'A' in row:
                a,branch=apair(w['AD'],w['AC'],picks['AD'],picks['AC'],b,scale)
                verify(a,row['A'],(file.name,row['sequence'],'A'))
                assert branch==row['A_feature_branch'];verifiedA+=1
                if a is not None:
                    assert row['D']==a['lag']-b['lag']
                    assert row['G']==.5*(b['early_feature']+b['late_feature']-a['early_feature']-a['late_feature'])
            r=clean(row);r['picks']=picks;r['B_search']=detail
            if b is not None:
                lags=np.arange(max(1,math.floor(scale/5000)-5),math.ceil(scale/1800)+6)
                cs=correlations(w['BD'],w['BC'],b['early_feature'],lags)
                peaks=find_peaks(cs)[0]
                peaks=sorted(peaks,key=lambda i:cs[i],reverse=True)
                r['B_full_range_peaks']=[dict(lag=int(lags[i]),corr=float(cs[i])) for i in peaks[:5]]
                r['B_global_peak']=dict(lag=int(lags[np.argmax(cs)]),corr=float(np.max(cs)))
                # Sensitivity diagnostics only; no new A/gates/state evaluation.
                variants={}
                for name,ratio,k in [('ratio10',.1,4),('ratio30',.3,4),('sigma2',.2,2),('sigma6',.2,6)]:
                    be=arrival(w['BD'],ratio,k);bl=arrival(w['BC'],ratio,k)
                    alt,_=pair(w['BD'],w['BC'],be,bl,scale)
                    variants[name]=alt
                r['B_sensitivity']=variants
                if row.get('decision')=='accepted' and cfg['G_min']==-12:
                    band='low' if b['sos']<3000 else 'mid' if b['sos']<3750 else 'near'
                    # Deterministic example: highest A quality per band, plus one frame per completed session.
                    key=band
                    if key not in examples or row['A']['corr']>examples[key]['row']['A']['corr']:
                        examples[key]=dict(file=file.name,row=r,waves={k:v.tolist() for k,v in w.items()},lags=lags.tolist(),corr=cs.tolist())
                    if row['sequence']>=15 and key+'_'+file.name[15:21] not in examples:
                        examples[key+'_'+file.name[15:21]]=dict(file=file.name,row=r,waves={k:v.tolist() for k,v in w.items()},lags=lags.tolist(),corr=cs.tolist())
            fr.append(r)
        assert hashlib.sha256(file.read_bytes()).hexdigest()==sha
        manifest.append(dict(file=file.name,sha256=sha,bytes=len(data),config=cfg))
        out.append(dict(file=file.name,config=cfg,frames=fr,events=[r for r in rows if r['event']!='frame']))
        print('verified',file.name,len(fr),flush=True)
    result=dict(scope='Independent raw feature reconstruction, not full Qt state-machine replay or physical truth.',
        verified_B=count,verified_A=verifiedA,manifest=manifest,files=out,
        source_hashes={s:hashlib.sha256((ROOT/s).read_bytes()).hexdigest() for s in ['src/signalprocessor.cpp','src/mainwindow.cpp','include/types.h']})
    (OUT/'results.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
    (OUT/'examples.json').write_text(json.dumps(examples),encoding='utf-8')
    print('DONE',count,verifiedA)

if __name__=='__main__':
    main()
