"""Read-only recorded-trajectory analysis; no device, clinical XML or product edits.

Run from the project root: python docs/research/g-range-onset-20260918/analyze.py
Requires numpy/scipy and the two previously verified raw-feature caches.
Candidate G results stop at each original recording endpoint, not new sessions.
"""
from pathlib import Path
import sys, json, hashlib, collections, copy
import numpy as np

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT/'docs/research/measurement-wave-audit-20260915'))
import analyze as wave
from replay import State, trimmed
OUT = Path(__file__).resolve().parent
PANEL = [(-12,0),(-10,0),(-8,0),(-6,0),(-12,-2),(-10,-2),(-8,2),(-6,2),(-6,6),(-12,2)]
SOURCES = ['src/mainwindow.cpp','src/signalprocessor.cpp','include/types.h','include/mainwindow.h']

def sha(p): return hashlib.sha256(p.read_bytes()).hexdigest()

def replay(f, bounds=None, force_guard=False, verify=False):
    c=copy.copy(f['config'])
    if bounds is not None: c['G_min'],c['G_max']=bounds
    state=State(c); accepted=[]; summary=None; completion=None
    for r in f['frames']:
        guard=force_guard or c.get('B_onset_forward_limit',0)>0
        bad=guard and any(p and p['onset']-p['hit']>40 for ch in ('BD','BC') if (p:=r['picks'][ch]))
        base=False; counted=False
        if bad or 'gates' not in r:
            state.reject()
        else:
            b,a=r['B'],r['A']; d=a['lag']-b['lag']
            base=(abs(b['lag']-b['rough_lag'])<=70 and b['lag']<260 and abs(d)<=18 and d>=-2
                  and a['corr']>=c['frame_corr_A'] and b['corr']>=c['frame_corr_B']
                  and c['D_min']<=d<=c['D_max'])
            stable=state.stable(b['lag']) if base else False
            if not base: state.reject()
            counted=base and c['G_min']<=r['G']<=c['G_max'] and stable
        if verify:
            assert counted==(r['decision']=='accepted'), (f['file'],r['sequence'],'decision')
            if 'gates' in r:
                assert base==r['gates']['stability_evaluated']
                assert (state.locked,state.center,len(state.values))==(r['locked'],r['locked_lag'],r['partial_values_before_accept']), (f['file'],r['sequence'],'state')
        if counted:
            accepted.append({'sequence':r['sequence'],'sos':r['sos_patient'],'G':r['G']})
            state.values.append(r)
            if len(state.values)>=c['frame_target']:
                summary={k:trimmed([get(v) for v in state.values]) for k,get in {
                    'sos':lambda x:x['sos_patient'],'corr_A':lambda x:x['A']['corr'],
                    'corr_B':lambda x:x['B']['corr'],'D':lambda x:x['D'],'G':lambda x:x['G']}.items()}
                summary['quality_pass']=(summary['corr_A']>=c['round_corr_A'] and summary['corr_B']>=c['round_corr_B']
                    and c['D_min']<=summary['D']<=c['D_max'] and c['G_min']<=summary['G']<=c['G_max'])
                completion={'ms':r['elapsed_ms'],'sequence':r['sequence']};break
    if verify:
        expected=next((r for r in f['events'] if r['event']=='round_summary'),None)
        assert (summary is None)==(expected is None),(f['file'],'presence')
        if summary:
            assert all(abs(v-expected[k])<1e-8 for k,v in summary.items()),(f['file'],'summary')
            assert completion['sequence']+1==expected['sequence']
    return dict(file=f['file'],accepted=accepted,summary=summary,completion=completion)

def describe(a):
    return dict(n=len(a),min=min(a),p10=float(np.quantile(a,.1)),median=float(np.median(a)),p90=float(np.quantile(a,.9)),max=max(a)) if a else {'n':0}

def summarize(cases,baseline):
    values=[r['sos'] for c in cases for r in c['accepted']]
    summaries=[c['summary']['sos'] for c in cases if c['summary']]
    changes=[]
    for b,c in zip(baseline,cases):
        if (b['summary'],b['completion'])!=(c['summary'],c['completion']):
            changes.append(dict(file=c['file'],before=b['summary'],after=c['summary'],before_finish=b['completion'],after_finish=c['completion']))
    return dict(accepted=describe(values),accepted_below3000=sum(x<3000 for x in values),accepted_above4200=sum(x>4200 for x in values),
        rounds=describe(summaries),passing_rounds=sum(bool(c['summary'] and c['summary']['quality_pass']) for c in cases),
        lost=sum(bool(b['summary'] and not c['summary']) for b,c in zip(baseline,cases)),
        gained=sum(bool(c['summary'] and not b['summary']) for b,c in zip(baseline,cases)),
        later=sum(bool(b['completion'] and c['completion'] and c['completion']['ms']>b['completion']['ms']) for b,c in zip(baseline,cases)),changes=changes)

def main():
    source_hashes={s:sha(ROOT/s) for s in SOURCES}
    groups={};manifest=[]
    for date in ('20260914','20260915'):
        cache=ROOT/f'build/wave-audit-{date}/results.json'
        data=json.loads(cache.read_text(encoding='utf-8'))
        for m in data['manifest']:
            p=ROOT/'build/Desktop_Qt_6_5_3_MinGW_64_bit_Debug/debug/measurement-experiments'/m['file']
            assert sha(p)==m['sha256'];manifest.append({'path':str(p.relative_to(ROOT)),'sha256':m['sha256']})
        groups[date+'-old']=data['files']
    fresh=[]; sensitivities=collections.defaultdict(list); search=[]
    for p in sorted((ROOT/'build/debug/debug/measurement-experiments').glob('*.jsonl')):
        raw=p.read_bytes();rows=[json.loads(l) for l in raw.splitlines() if l.strip()]
        cfg=rows[0].get('config',{})
        if cfg.get('implementation')!='onset-consistency-20260915-v1':continue
        assert rows[-1]['event']=='stop'
        assert [r['sequence'] for r in rows]==list(range(1,len(rows)+1))
        manifest.append({'path':str(p.relative_to(ROOT)),'sha256':hashlib.sha256(raw).hexdigest()})
        fr=[];scale=cfg['probe_distance_m']/cfg['sample_period_s']
        for row in rows:
            if row['event']!='frame': continue
            w={ch:wave.filtered(row,ch) for ch in ('BD','BC','AD','AC')}
            picks={ch:wave.arrival(x) for ch,x in w.items()}
            for ch in ('BD','BC'):
                pck=picks[ch];logged=row['B_arrivals'][ch]
                assert (pck is not None)==logged['valid']
                if pck: assert (pck['hit'],pck['onset'],pck['peak'])==(logged['first_hit'],logged['onset'],logged['peak'])
            bad=any(picks[ch] and picks[ch]['onset']-picks[ch]['hit']>40 for ch in ('BD','BC'))
            assert bad==(row['decision']=='B_onset_inconsistent')
            b,detail=wave.pair(w['BD'],w['BC'],picks['BD'],picks['BC'],scale)
            if not bad:wave.verify(b,row['B'],(p.name,row['sequence'],'B'))
            if 'A' in row:
                a,branch=wave.apair(w['AD'],w['AC'],picks['AD'],picks['AC'],b,scale)
                wave.verify(a,row['A'],(p.name,row['sequence'],'A'))
                if a:
                    assert row['D']==a['lag']-b['lag']
                    assert row['G']==.5*(b['early_feature']+b['late_feature']-a['early_feature']-a['late_feature'])
            r=wave.clean(row);r['picks']=picks;r['B_search']=detail;fr.append(r)
            if row['decision']=='accepted':
                band='below3750' if b['sos']<3750 else 'atleast3750'
                search.append(dict(file=p.name,sequence=row['sequence'],band=band,G=row['G'],sos=b['sos'],rough=b['rough_lag'],lag=b['lag'],**detail))
                for name,ratio in [('ratio10',.1),('ratio30',.3),('first_hit',None)]:
                    pp={ch:wave.arrival(w[ch],ratio) if ratio else dict(picks[ch],onset=picks[ch]['hit']) for ch in ('BD','BC')}
                    alt,_=wave.pair(w['BD'],w['BC'],pp['BD'],pp['BC'],scale)
                    sensitivities[name].append(dict(band=band,before=b['sos'],after=alt['sos'] if alt else None))
        fresh.append(dict(file=p.name,config=cfg,frames=fr,events=[r for r in rows if r['event']!='frame']))
        print('raw verified',p.name,len(fr),flush=True)
    groups['20260915-new']=fresh
    output={'scope':'Retrospective replica, not Qt execution or accuracy truth. Same recorded endpoints. G panel keeps onset guard40 and other per-file settings. SOS bands are descriptive, not labels. Onset variants are B-only sensitivity, not deployable full pipeline.',
            'manifest':manifest,'source_hashes':source_hashes,'groups':{},'onset_sensitivity':{},'search':{}}
    # New logs contain five uncomplicated, exactly-five-round sessions. Preserve
    # their recorded grouping only; changed operator actions are not simulated.
    output['conditional_final_sessions']=[]
    session=[]
    for f in fresh:
        if any(e['event']=='round_summary' for e in f['events']):session.append(f)
        final=next((e for e in f['events'] if e['event']=='final_round_selection'),None)
        if final:
            assert len(session)==5 and final['selected_indices']==list(range(5))
            original=[replay(q,verify=True)['summary']['sos'] for q in session]
            assert abs(sum(original)/5-final['final_sos'])<1e-8
            candidates={}
            for bounds in PANEL:
                cases=[replay(q,bounds,True) for q in session]
                ss=[c['summary']['sos'] for c in cases if c['summary'] and c['summary']['quality_pass']]
                candidates[str(bounds)]={'retained_rounds':len(ss),'conditional_final':sum(ss)/5 if len(ss)==5 else None,'rounds':ss}
            output['conditional_final_sessions'].append({'final_file':f['file'],'original':final['final_sos'],'candidates':candidates})
            session=[]
    for name,files in groups.items():
        original=[replay(f,verify=True) for f in files]
        baseline=[replay(f,(-12,0),True) for f in files]
        panel={str(bounds):summarize([replay(f,bounds,True) for f in files],baseline) for bounds in PANEL}
        distributions={}
        for band,low,high in [('below3000',0,3000),('3000to3750',3000,3750),('3750to3950',3750,3950),('above3950',3950,10000)]:
            accepted=[r['G'] for f in files for r in f['frames'] if r['decision']=='accepted' and low<=r['sos_patient']<high]
            eligible=[r['G'] for f in files for r in f['frames'] if r.get('gates',{}).get('stability_evaluated') and low<=r['sos_patient']<high and not any(p and p['onset']-p['hit']>40 for ch in ('BD','BC') if (p:=r['picks'][ch]))]
            distributions[band]={'original_accepted_G':describe(accepted),'basic_quality_G_before_G_stability':describe(eligible)}
        output['groups'][name]=dict(files=len(files),frames=sum(len(f['frames']) for f in files),original=summarize(original,original),panel=panel,G_distribution=distributions)
        print('STATE BASELINE matched',name,flush=True)
    for name,rows in sensitivities.items():
        output['onset_sensitivity'][name]={band:dict(after=describe([r['after'] for r in rows if r['band']==band and r['after'] is not None]),delta=describe([r['after']-r['before'] for r in rows if r['band']==band and r['after'] is not None]),invalid=sum(r['after'] is None for r in rows if r['band']==band)) for band in ('below3750','atleast3750')}
    for band in ('below3750','atleast3750'):
        rows=[r for r in search if r['band']==band]
        output['search'][band]=dict(n=len(rows),edge=sum(r['edge'] for r in rows),rough=describe([r['rough'] for r in rows]),lag=describe([r['lag'] for r in rows]),G=describe([r['G'] for r in rows]))
    assert all(sha(ROOT/m['path'])==m['sha256'] for m in manifest)
    assert all(sha(ROOT/s)==h for s,h in source_hashes.items())
    (OUT/'results.json').write_text(json.dumps(output,ensure_ascii=False,indent=2),encoding='utf-8')
    import matplotlib
    matplotlib.use('Agg')
    import matplotlib.pyplot as plt
    fig,axes=plt.subplots(1,2,figsize=(11,4.2),layout='constrained')
    for band,label,color in [('below3750','Accepted SOS < 3750','#d87926'),('atleast3750','Accepted SOS >= 3750','#2166ac')]:
        rr=[r for r in search if r['band']==band]
        axes[0].scatter([r['G'] for r in rr],[r['sos'] for r in rr],s=17,alpha=.28,label=label,color=color)
    axes[0].set(xlabel='G (sample points)',ylabel='Accepted frame SOS (m/s)',title='G overlaps across SOS groups')
    axes[0].legend(fontsize=8);axes[0].grid(alpha=.2)
    selected=[(-12,0),(-10,0),(-8,0),(-6,0),(-12,2)]
    values=[output['groups']['20260915-new']['panel'][str(b)]['rounds']['n'] for b in selected]
    axes[1].bar([str(b) for b in selected],values,color=['#667788']*4+['#2166ac'])
    for i,v in enumerate(values):axes[1].text(i,v+.3,str(v),ha='center')
    axes[1].set(ylim=(0,28),ylabel='Completed rounds within recorded endpoints',title='New 26 recordings; onset guard fixed at 40')
    axes[1].tick_params(axis='x',labelsize=8)
    fig.savefig(OUT/'g-comparison.png',dpi=160)
    print('DONE preserved inputs and product sources',len(manifest),flush=True)

if __name__=='__main__': main()
