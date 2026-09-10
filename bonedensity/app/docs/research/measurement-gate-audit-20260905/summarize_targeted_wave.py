"""Summarize validated raw diagnostics and independently audit logged clusters."""
import json
import pathlib
import statistics as st
import sys


def best_cluster(candidates, tolerance):
    best = []
    for center in candidates:
        subset = [v for v in candidates if abs(v - center) <= tolerance]
        if len(subset) > len(best) or (len(subset) == len(best) and subset and best
                and sum(abs(v - st.median(subset)) for v in subset)
                < sum(abs(v - st.median(best)) for v in best)):
            best = subset
    return best


def main(raw_path, triage_path):
    raw = json.loads(pathlib.Path(raw_path).read_text())['files']
    triage = json.loads(pathlib.Path(triage_path).read_text(encoding='utf-8'))['files']
    by_name = {f['file']: f for f in triage}
    retained = []
    anchors = []
    for f in raw:
        assert f['sha256'] == by_name[f['file']]['sha256']
        kept = [r for r in f['frames'] if r['sequence'] in f['kept_sequences']]
        if kept:
            assert len(kept) == 30
            sos = sorted(r['B']['sos'] for r in kept)
            assert abs(st.mean(sos[6:-6]) - by_name[f['file']]['summaries'][0]['sos']) < 1e-7
        for r in kept:
            if r['B_peak']['edge']:
                x = r['B_extended_same_platform']
                retained.append(dict(file=f['file'], sequence=r['sequence'],
                    original_sos=r['B']['sos'], original_lag=r['B']['lag'],
                    search=[r['B_search_low'],r['B_search_high']], peak=r['B_peak'],
                    extended_peak=r['B_peak_extended15'], same_platform=x,
                    isolated_sos_delta=490000/x['lag']-r['B']['sos']))
        for r in f['frames']:
            a = r.get('one_step_trusted_anchor')
            if a and a['continuity_eligible']:
                anchors.append(dict(file=f['file'], sequence=r['sequence'],
                    original_A=r['A'], original_G=r['G'], original_D=r['D'],
                    original_gates=r['gates'], B_sos=r['B']['sos'],
                    AD_similarity=r['AD_same_coordinate_corr'],
                    AC_similarity=r['AC_same_coordinate_corr'], candidate=a))
    checked = 0
    candidates = []
    overfive = []
    for f in triage:
        if f['round'] == 1:
            candidates = []
        summaries = f['summaries'] + [e for e in f['unverified_suffix_events'] if e['event']=='round_summary']
        clusters = f['clusters'] + [e for e in f['unverified_suffix_events'] if e['event']=='round_cluster']
        assert len(summaries) == len(clusters)
        for s, c in zip(summaries, clusters):
            if s['quality_pass']:
                candidates.append(s['sos'])
            computed = best_cluster(candidates, 180)
            assert len(computed)==len(c['accepted_sos']) and all(abs(a-b)<1e-7 for a,b in zip(computed,c['accepted_sos']))
            checked += 1
            if len(computed)>5:
                sorted_values=sorted(computed)
                windows=[sorted_values[i:i+5] for i in range(len(computed)-4)]
                proposed=min(windows,key=lambda a:(a[-1]-a[0],sum(abs(v-st.median(a)) for v in a)))
                overfive.append(dict(file=f['file'],original=computed,original_final=st.mean(computed),
                    proposed_five=proposed,proposed_final=st.mean(proposed)))
    return dict(scope='Raw selected-frame diagnostics and cluster reproduction, NOT full candidate replay or clinical accuracy.',
        selected_files=len(raw),selected_frames=sum(len(f['frames']) for f in raw),
        retained_frames=sum(len(f['kept_sequences']) for f in raw),
        retained_boundary_frames=len(retained),boundary_rounds=len(set(x['file'] for x in retained)),
        extended_still_boundary=sum(x['extended_peak']['edge'] for x in retained),
        same_platform_changed=sum(x['same_platform']['lag']!=x['original_lag'] for x in retained),
        same_platform_median_abs_delta=st.median(abs(x['isolated_sos_delta']) for x in retained),
        same_platform_max_abs_delta=max(abs(x['isolated_sos_delta']) for x in retained),
        trusted_anchor_eligible=len(anchors),trusted_anchor_ADG_pass=sum(x['candidate']['A_D_G_pass'] for x in anchors),
        anchors=anchors,boundary_examples=sorted(retained,key=lambda x:abs(x['isolated_sos_delta']),reverse=True)[:4],
        reproduced_clusters=checked,overfive=overfive)


if __name__ == '__main__':
    print(json.dumps(main(sys.argv[1],sys.argv[2]),ensure_ascii=False,indent=2))
