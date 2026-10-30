"""Read-only CSV investigation. No clinical labels inferred from file names.

Run with bundled Python and --input-root <attachment directory>.
Only --output is written. CSV source bytes are hash-checked after analysis.
No pipeline replay: these legacy files omit raw waves and acceptance events.
"""
import argparse
import hashlib
import json
from pathlib import Path

import numpy as np
import pandas as pd


def dist(v):
    a = np.asarray(v, dtype=float)
    a = a[np.isfinite(a)]
    if not len(a):
        return None
    return dict(n=len(a), **{k: float(x) for k, x in zip(
        ['min', 'p10', 'q25', 'median', 'q75', 'p90', 'max'],
        np.quantile(a, [0, .1, .25, .5, .75, .9, 1]))})


def rho(a, b):
    # Spearman is Pearson correlation of average ranks; avoids optional SciPy.
    pair=pd.concat([a,b],axis=1).dropna()
    return float(np.corrcoef(pair.rank().to_numpy().T)[0,1]) if len(pair)>1 and (pair.nunique()>1).all() else None


def gates(d):
    return pd.DataFrame({
        'B_jump': (d.lagB - (d.bcOnset-d.bdOnset)).abs() <= 70,
        'boundary': d.lagB < 260,
        'AB_diff': (d.lagA-d.lagB).abs() <= 18,
        'direction': d.lagA-d.lagB >= -2,
        'A_quality': d.corrA >= .78, 'B_quality': d.corrB >= .55,
        'D': d.signedLagDiff.between(5, 15), 'G': d.pairMidGap.between(-6, 6),
    }, index=d.index)


def describe(d):
    t = pd.to_datetime(d.time)
    dt = t.diff().dt.total_seconds()
    g = gates(d)
    core = g.drop(columns=['D', 'G']).all(axis=1)
    all_pass = g.all(axis=1)
    no_g = g.drop(columns='G').all(axis=1)
    out = dict(rows=len(d), csv_rows=[int(d.index[0]+2), int(d.index[-1]+2)],
        start=str(t.iloc[0]), end=str(t.iloc[-1]), modes=d['mode'].value_counts().to_dict(),
        time_backwards=int((dt < 0).sum()), gaps_over_2s=int((dt > 2).sum()),
        missing_cells=int(d.isna().sum().sum()), interval_s=dist(dt),
        features={k: dist(d[k]) for k in ['sosAvg','sosA','sosB','lagA','lagB','signedLagDiff','pairMidGap','corrA','corrB']},
        correlations={'G_B': rho(d.pairMidGap,d.sosB), 'G_avg': rho(d.pairMidGap,d.sosAvg),
                      'D_B': rho(d.signedLagDiff,d.sosB), 'D_G': rho(d.signedLagDiff,d.pairMidGap)},
        arithmetic={'D_exact': bool(np.allclose(d.signedLagDiff,d.lagA-d.lagB)),
                    'G_exact': bool(np.allclose(d.pairMidGap,d.pairMidB-d.pairMidA)),
                    'SOS_equals_B': int(np.isclose(d.sosAvg,d.sosB,atol=.02,rtol=0).sum()),
                    'SOS_equals_weighted': int(np.isclose(d.sosAvg,d.wB*d.sosB+(1-d.wB)*d.sosA,atol=.02,rtol=0).sum()),
                    'weighted_minus_B': dist(d.sosAvg-d.sosB),
                    'B_mid_matches_onsets': int(np.isclose(d.pairMidB,(d.bcOnset+d.bdOnset)/2).sum())},
        descriptive_old_score_gates={'all': int(all_pass.sum()), 'no_G': int(no_g.sum()),
                    'no_DG': int(core.sum()),
                    'individual_fail': {k:int((~g[k]).sum()) for k in g},
                    'sole_fail': {k:int((~g[k] & g.drop(columns=k).all(axis=1)).sum()) for k in g},
                    'all_SOS_B': dist(d.loc[all_pass,'sosB']),
                    'G_only_rejected_SOS_B': dist(d.loc[no_g & ~g.G,'sosB']),
                    'G_among_other_gates_passing': dist(d.loc[no_g,'pairMidGap'])})
    landmarks={}
    for name, field, cols in [
        ('onset','onsetMidGap',['bcOnset','bdOnset','acOnset','adOnset']),
        ('peak','peakMidGap',['bcPeak','bdPeak','acPeak','adPeak']),
        ('valley','valleyMidGap',['bcValley','bdValley','acValley','adValley'])]:
        valid=(d[cols]>=0).all(axis=1)
        landmarks[name]={'valid_indices':int(valid.sum()), 'G':dist(d.loc[valid,field]),
                         'rho_B':rho(d.loc[valid,field],d.loc[valid,'sosB'])}
    out['debug_landmark_diagnostics_not_replacement_features']=landmarks
    near=(dt>0)&(dt<=.2)&(d['mode']==d['mode'].shift())
    jumps=near & (d.lagB.diff()==0) & (d.pairMidB.diff().abs()<=2) & (d.pairMidGap.diff().abs()>=20)
    out['adjacent_equal_B_lag_G_jump20']={'count':int(jumps.sum()), 'examples':[]}
    for idx in d.index[jumps][:4]:
        pos=d.index.get_loc(idx)
        s=d.iloc[pos-1:pos+1]
        out['adjacent_equal_B_lag_G_jump20']['examples'].append(
            {'csv_rows':[int(i+2) for i in s.index], **{k:s[k].tolist() for k in ['time','sosB','pairMidGap','pairMidA','pairMidB','corrA']}})
    return out


def segments(d):
    dt=pd.to_datetime(d.time).diff().dt.total_seconds()
    group=((dt>2)|(dt<=0)|(d['mode']!=d['mode'].shift())).cumsum()
    result=[]
    stable=[]
    for _,s in d.groupby(group):
        result.append({'csv_rows':[int(s.index[0]+2),int(s.index[-1]+2)],'n':len(s),
                       'start':s.time.iloc[0],'end':s.time.iloc[-1],
                       **{k:dist(s[k]) for k in ['sosAvg','sosB','pairMidGap','signedLagDiff','corrA','corrB']}})
        for start in range(0,len(s)-29,30):
            w=s.iloc[start:start+30]
            gap=pd.to_datetime(w.time).diff().dt.total_seconds().iloc[1:]
            if (gap>0).all() and (gap<=.5).all() and w.lagB.max()-w.lagB.min()<=2:
                stable.append({'csv_rows':[int(w.index[0]+2),int(w.index[-1]+2)],
                    'duration_s':float(gap.sum()),'lag_span':int(w.lagB.max()-w.lagB.min()),
                    **{k:dist(w[k]) for k in ['sosAvg','sosB','pairMidGap','signedLagDiff','corrA','corrB']}})
    return result,stable


def reference_comparison(d, reference):
    # A descriptive band around the user's approximate final reference only.
    # It is neither per-frame ground truth nor a proposed acceptance threshold.
    out={'approximate_reference':reference, 'bands':{}}
    for column in ['sosAvg','sosB']:
        near=(d[column]-reference).abs()<=100
        out['bands'][column]={'within_100_rows':int(near.sum()),
            'G':dist(d.loc[near,'pairMidGap']),
            'D':dist(d.loc[near,'signedLagDiff']),
            'A':dist(d.loc[near,'corrA']),
            'outside_G6':int((near & ~d.pairMidGap.between(-6,6)).sum()),
            'outside_D5_15':int((near & ~d.signedLagDiff.between(5,15)).sum())}
    return out


def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--input-root',type=Path,required=True)
    ap.add_argument('--output',type=Path,required=True)
    args=ap.parse_args()
    frames={p.name:pd.read_csv(p) for p in sorted(args.input_root.glob('*.csv'))}
    hashes={n:hashlib.sha256((args.input_root/n).read_bytes()).hexdigest() for n in frames}
    expected=['angle_features cu.csv','angle_features plastic.csv','angle_features yrc.csv',
        'angle_features.csv','angle_features_angle.csv','angle_features_haj_5round.csv',
        'angle_features_lhy.csv','angle_features_lhy_round5.csv','angle_features_location.csv',
        'angle_features_wyl_2.csv','angle_features_wyl_3_5round.csv']
    assert sorted(frames)==sorted(expected),'Unexpected input inventory'
    rows={n:set(pd.util.hash_pandas_object(d,index=False)) for n,d in frames.items()}
    overlap=[]
    for i,a in enumerate(frames):
        for b in list(frames)[i+1:]:
            common=len(rows[a]&rows[b])
            if common: overlap.append({'a':a,'b':b,'exact_common_rows':common})
    slices={n:(n,0) for n in frames}
    for child,parent in [('angle_features cu.csv','angle_features yrc.csv'),
            ('angle_features plastic.csv','angle_features cu.csv'),
            ('angle_features_haj_5round.csv','angle_features_lhy_round5.csv')]:
        count=len(frames[parent])
        assert frames[child].iloc[:count].reset_index(drop=True).equals(frames[parent])
        del slices[child]
        slices[child+' [new suffix; identity unconfirmed]']=(child,count)
    output={'scope':'Descriptive CSV audit only. Old A scores, mixed SOS conventions, no raw waveforms, no round events. No accuracy or full-flow replay claim.',
            'total_rows_in_files':sum(map(len,frames.values())),
            'unique_exact_rows':len(set().union(*rows.values())),
            'overlaps':overlap,'files':{},'slices':{},'reference_comparisons':{},'selected_excerpts':{}}
    for n,d in frames.items():
        output['files'][n]={'sha256':hashes[n],**describe(d)}
    for label,(n,start) in slices.items():
        d=frames[n].iloc[start:]
        seg,stable=segments(d)
        output['slices'][label]={'source':n,**describe(d),'segments':seg,
            'nonoverlapping_30row_B_span_le2_windows':stable}
    for n,ref,start in [('angle_features_wyl_2.csv',3850,0),
            ('angle_features_wyl_3_5round.csv',3850,0),
            ('angle_features_lhy.csv',3800,0),('angle_features_lhy_round5.csv',3800,0),
            ('angle_features_haj_5round.csv',3800,2272)]:
        output['reference_comparisons'][n]=reference_comparison(frames[n].iloc[start:],ref)
    # Final YRC session is tentative: historical prefix is not labelled by person.
    yrc=frames['angle_features yrc.csv']
    recent=yrc[pd.to_datetime(yrc.time)>=pd.Timestamp('2026-09-08 19:38:00')]
    output['reference_comparisons']['yrc latest session; identity unconfirmed']=reference_comparison(recent,3900)
    for n,first,last in [('angle_features_lhy_round5.csv',308,427),
            ('angle_features_wyl_3_5round.csv',1001,1060),
            ('angle_features_haj_5round.csv',4660,4689),
            ('angle_features_wyl_2.csv',1082,1111)]:
        output['selected_excerpts'][f'{n}:{first}-{last}']=describe(frames[n].iloc[first-2:last-1])
    assert all(hashlib.sha256((args.input_root/n).read_bytes()).hexdigest()==h for n,h in hashes.items())
    output['source_hashes_unchanged']=True
    args.output.write_text(json.dumps(output,ensure_ascii=False,indent=2,allow_nan=False),encoding='utf-8')
    print(json.dumps({'files':len(frames),'rows':output['total_rows_in_files'],
                     'unique_rows':output['unique_exact_rows'],'source_hashes_unchanged':True}))


if __name__=='__main__':
    main()
