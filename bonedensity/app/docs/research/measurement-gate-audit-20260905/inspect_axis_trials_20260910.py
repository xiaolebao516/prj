"""Descriptive axis-trial audit; no CSV/product changes or acceptance replay."""
import argparse
import hashlib
import json
import sys
from pathlib import Path

sys.dont_write_bytecode = True
import numpy as np
import pandas as pd
from inspect_legacy_batch_20260910 import describe, dist


def analyze(d):
    result = describe(d)
    t = pd.to_datetime(d.time)
    dt = t.diff().dt.total_seconds()
    result['duration_s'] = float((t.iloc[-1]-t.iloc[0]).total_seconds())
    result['gaps_over_1s'] = [dict(csv_row=int(i+2), seconds=float(x))
                            for i,x in dt.items() if x>1]
    quality = (d.corrA>=.78)&(d.corrB>=.55)
    jump = dt.between(.001,.2)&d.lagB.diff().eq(0)&d.pairMidB.diff().abs().le(2)&d.pairMidGap.diff().abs().ge(20)
    result['same_B_jump'] = {
        'count':int(jump.sum()),
        'absolute_G_change':dist(d.pairMidGap.diff().abs()[jump]),
        'both_frames_old_quality_pass':int((jump&quality&quality.shift(fill_value=False)).sum())}
    result['old_quality_subset'] = {'n':int(quality.sum()),
        **{c:dist(d.loc[quality,c]) for c in ['sosAvg','sosB','pairMidGap','signedLagDiff']}}
    near = d.sosB.between(3700,3900)
    result['B_3700_3900_descriptive_not_truth'] = {'n':int(near.sum()),
        'G':dist(d.loc[near,'pairMidGap']), 'old_A':dist(d.loc[near,'corrA'])}
    result['five_second_bins_not_action_labels'] = []
    groups = (dt>5).cumsum()
    for _,s in d.groupby(groups):
        seconds=(pd.to_datetime(s.time)-pd.to_datetime(s.time.iloc[0])).dt.total_seconds()
        for _,w in s.groupby((seconds//5).astype(int)):
            result['five_second_bins_not_action_labels'].append({
                'csv_rows':[int(w.index[0]+2),int(w.index[-1]+2)],
                'time':[w.time.iloc[0],w.time.iloc[-1]],'n':len(w),
                **{c:dist(w[c]) for c in ['sosAvg','sosB','signedLagDiff','pairMidGap','corrA','corrB']}})
    return result


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--input-root',type=Path,required=True)
    parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args()
    names=['angle_features_long.csv','angle_features_short.csv','angle_features.csv']
    hashes={n:hashlib.sha256((args.input_root/n).read_bytes()).hexdigest() for n in names}
    data={n:pd.read_csv(args.input_root/n) for n in names}
    long=data[names[0]];short=data[names[1]];old=data[names[2]]
    assert len(old)==1494 and long.iloc[:len(old)].equals(old)
    assert len(long)==2041 and len(short)==575
    for d in [long,short]:
        assert np.allclose(d.signedLagDiff,d.lagA-d.lagB)
        assert np.allclose(d.pairMidGap,d.pairMidB-d.pairMidA)
        assert np.allclose(d.sosAvg,d.wB*d.sosB+(1-d.wB)*d.sosA,rtol=0,atol=.02)
    current=long.iloc[len(old):]
    main_long=current[pd.to_datetime(current.time)>=pd.Timestamp('2026-09-10 17:17:00')]
    output={'scope':'Old weighted SOS/old correlation scores; no raw waves, precise motion labels or actual round replay. 3800 is user-reported initial alignment, not per-frame truth.',
        'sha256':hashes,'long_exact_historical_prefix_rows':len(old),
        'long_all_new':analyze(current),'long_recent_463':analyze(main_long),
        'short_all':analyze(short),
        'long_consecutive_example_csv_rows_1935_1937':long.iloc[1933:1936][[
            'time','sosAvg','sosB','lagA','lagB','pairMidA','pairMidB','pairMidGap','corrA','corrB']].to_dict('records')}
    assert all(hashlib.sha256((args.input_root/n).read_bytes()).hexdigest()==h for n,h in hashes.items())
    output['source_hashes_unchanged']=True
    args.output.write_text(json.dumps(output,ensure_ascii=False,indent=2,allow_nan=False),encoding='utf8')
    print(json.dumps({'long_new':len(current),'long_recent':len(main_long),'short':len(short),
        'same_B_G_jumps_long':output['long_recent_463']['same_B_jump'],
        'same_B_G_jumps_short':output['short_all']['same_B_jump'],'source_hashes_unchanged':True}))


if __name__=='__main__':
    main()
