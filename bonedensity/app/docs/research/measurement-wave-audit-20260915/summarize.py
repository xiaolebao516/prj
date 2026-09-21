"""Create compact evidence and inspectable figures from validated audit output."""
from pathlib import Path
import json
import base64
import hashlib
import collections
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib import font_manager
from analyze import ROOT, INPUT, envelope

OUT=Path(__file__).resolve().parent
font=Path('C:/Windows/Fonts/msyh.ttc')
if font.exists():
    font_manager.fontManager.addfont(str(font))
    plt.rcParams['font.family']=font_manager.FontProperties(fname=str(font)).get_name()
plt.rcParams['axes.unicode_minus']=False
plt.rcParams['font.size']=10

def range3(xs):
    return [float(np.min(xs)),float(np.median(xs)),float(np.max(xs))] if xs else []

def main():
    report={'scope':'Retrospective engineering evidence; historical 3850 is not paired truth.', 'days':{}}
    for date in ['20260914','20260915']:
        work=ROOT/('build/wave-audit-'+date)
        x=json.loads((work/'results.json').read_text(encoding='utf8'))
        replay=json.loads((work/'state-replay.json').read_text(encoding='utf8'))
        for path,digest in x['source_hashes'].items():
            assert hashlib.sha256((ROOT/path).read_bytes()).hexdigest()==digest
        for f in x['files']:
            rows=sorted(f['frames']+f['events'],key=lambda r:r['sequence'])
            assert all(a['elapsed_ms']<=b['elapsed_ms'] for a,b in zip(rows,rows[1:]))
        acc=[r for f in x['files'] for r in f['frames'] if r['decision']=='accepted']
        low=[r for r in acc if r['sos_patient']<3000]
        other=[r for r in acc if r['sos_patient']>=3000]
        candidates={}
        for limit,scenario in replay['scenarios'].items():
            candidates[limit]={'accepted':sum(len(c['accepted']) for c in scenario['cases']),
                'low_accepted':sum(r['sos']<3000 for c in scenario['cases'] for r in c['accepted']),
                'changed_rounds':scenario['changed_rounds']}
        report['days'][date]={'files':len(x['files']),'B_verified':x['verified_B'],'A_verified':x['verified_A'],
            'accepted':len(acc),'low_accepted':len(low),
            'low_BC_forward_shift':range3([r['picks']['BC']['onset']-r['picks']['BC']['hit'] for r in low]),
            'other_B_max_forward_shift':max(r['picks'][ch]['onset']-r['picks'][ch]['hit'] for r in other for ch in ['BD','BC']),
            'rounds':sum(c['summary'] is not None for c in replay['baseline']),
            'passed_rounds':sum(bool(c['summary'] and c['summary']['quality_pass']) for c in replay['baseline']),
            'candidates':candidates,'source_hashes':x['source_hashes'],'manifest':x['manifest']}
        for m in x['manifest']:
            assert hashlib.sha256((INPUT/m['file']).read_bytes()).hexdigest()==m['sha256']

    x=json.loads((ROOT/'build/wave-audit-20260915/results.json').read_text(encoding='utf8'))
    latest=[f for f in x['files'] if f['config']['G_min']==-12]
    acc=[r for f in latest for r in f['frames'] if r['decision']=='accepted']
    report['latest_final_sos']=[r['final_sos'] for f in latest for r in f['events'] if r['event']=='final_round_selection']
    bands={}
    for name,lo,hi in [('low',0,3000),('mid',3000,3750),('near',3750,3950)]:
        rs=[r for r in acc if lo<=r['sos_patient']<hi]
        bands[name]={'n':len(rs),'search_peak_at_edge':sum(r['B_search']['edge'] for r in rs),
            'multiple_platform_components':sum(r['B_search']['components']>1 for r in rs),
            'full_range_argmax_sos':range3([490000/r['B_global_peak']['lag'] for r in rs]),
            'B_rough_lag':range3([r['B']['rough_lag'] for r in rs]),
            'B_final_lag':range3([r['B']['lag'] for r in rs])}
    # ADC rails are measured from original samples in a 141-point raw interval
    # shifted back by the FIR group delay (40); the filter has wider support.
    rails=collections.defaultdict(lambda:collections.defaultdict(list))
    for f in latest:
        raw={r['sequence']:r for r in map(json.loads,(INPUT/f['file']).read_bytes().splitlines()) if r['event']=='frame'}
        for r in f['frames']:
            if r['decision']!='accepted':continue
            v=r['sos_patient'];band='low' if v<3000 else 'mid' if v<3750 else 'near'
            for ch in ['BD','BC']:
                wave=np.frombuffer(base64.b64decode(raw[r['sequence']]['raw_'+ch]),dtype='<u2')
                start=r['B']['early_feature']-60+(r['B']['lag'] if ch=='BC' else 0)
                segment=wave[start:start+141]
                rails[band][ch].append(int(np.count_nonzero((segment==0)|(segment==4095))))
    report['latest_bands']=bands
    report['latest_raw_rails']={band:{ch:{'frames_with_rail':sum(v>0 for v in vals),'points_min_median_max':range3(vals)} for ch,vals in channels.items()} for band,channels in rails.items()}
    (OUT/'summary.json').write_text(json.dumps(report,ensure_ascii=False,indent=2),encoding='utf8')

    examples=json.loads((ROOT/'build/wave-audit-20260915/examples.json').read_text(encoding='utf8'))
    e=examples['low'];r=e['row'];bc=np.array(e['waves']['BC']);p=r['picks']['BC']
    fig,axs=plt.subplots(2,1,figsize=(11,7.6),layout='constrained')
    ax=axs[0];xx=np.arange(1100,1451);env=envelope(bc)
    ax.plot(xx,env[xx],color='#146c94',label='BC 滤波包络')
    ax.axhline(p['threshold'],color='#777',linestyle=':',label='噪声阈值')
    ax.axhline(p['level'],color='#b58000',linestyle=':',label='峰值20%回溯水平')
    ax.axvspan(p['hit'],p['hit']+140,color='#e7b45c',alpha=.17,label='后续140点内寻找最大包络值')
    for k,label,color,y in [('hit','首次越阈值','#20854e',.92),('onset','最终首波位置','#c63838',.73),('peak','选中包络峰','#855fa8',.9)]:
        ax.axvline(p[k],color=color,linestyle='--');ax.text(p[k]+2,env[xx].max()*y,f'{label}\n{p[k]}',color=color)
    ax.set(title='已计数低值实例：BC首波位置向后移动106点',xlabel='采样点',ylabel='包络幅度')
    ax.legend(loc='upper left',fontsize=8)
    ax=axs[1];lags=np.array(e['lags']);cs=np.array(e['corr']);d=r['B_search']
    ax.plot(lags,cs,color='#146c94',label='在原BD窗口计算的相关曲线')
    ax.axvspan(d['low'],d['high'],color='#e7b45c',alpha=.2,label=f"实际搜索范围 [{d['low']}, {d['high']}]")
    ax.axvline(r['B']['lag'],color='#c63838',linestyle='--',label=f"最终延迟 {r['B']['lag']} → SOS {r['sos_patient']:.0f}")
    for peak in r['B_full_range_peaks'][:3]:
        ax.annotate(f"lag {peak['lag']}\nr={peak['corr']:.3f}",(peak['lag'],peak['corr']),xytext=(peak['lag']-7,peak['corr']+.14),arrowprops={'arrowstyle':'-','color':'#666'},fontsize=9)
    ax.set(xlabel='两通道延迟（采样点）',ylabel='相关系数',ylim=(-1.05,1.25),title='前部峰不在实际搜索范围内；后部峰的相关性反而更高')
    ax.legend(loc='lower left',fontsize=9)
    fig.suptitle(f"{e['file'][6:28]} · sequence {r['sequence']} · A相关性 {r['A']['corr']:.3f}",fontsize=13)
    fig.savefig(OUT/'low-wave-localization.png',dpi=150);plt.close(fig)

    fig,axs=plt.subplots(2,3,figsize=(13,6.4),layout='constrained')
    for col,key in enumerate(['near_073215','mid_074150','low']):
        ex=examples[key];r=ex['row'];raw=next(v for v in map(json.loads,(INPUT/ex['file']).read_bytes().splitlines()) if v['sequence']==r['sequence'])
        for j,ch in enumerate(['BD','BC']):
            wave=np.frombuffer(base64.b64decode(raw['raw_'+ch]),dtype='<u2')
            ax=axs[j,col];idx=np.arange(900,1501);ax.plot(idx,wave[idx],lw=1,color='#146c94')
            ax.axhline(0,color='#c63838',ls=':',lw=.8);ax.axhline(4095,color='#c63838',ls=':',lw=.8)
            ax.set(ylim=(-150,4300),xlabel='原始采样点',ylabel=ch+' 原始码值')
            if j==0:ax.set_title(f"SOS {r['sos_patient']:.0f} / A相关性 {r['A']['corr']:.3f}\n{ex['file'][15:21]} · seq {r['sequence']}")
    fig.suptitle('原始波形触及码值边界：参考附近也存在，不能简单全部拒绝',fontsize=14)
    fig.savefig(OUT/'raw-rail-comparison.png',dpi=150);plt.close(fig)
    print('Summary and two figures written; all 75 input hashes and audited source hashes unchanged; timestamps monotonic.')

if __name__=='__main__':main()
