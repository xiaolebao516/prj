"""Attribute candidate gate failures on original accepted frames, not truth labels."""
from pathlib import Path
import json, collections
import candidates as c

def main():
    stats={n:collections.Counter() for n in c.NAMES};examples={}
    for p in sorted((c.ROOT/'build/debug/debug/measurement-experiments').glob('*.jsonl')):
        rows=[json.loads(l) for l in p.read_bytes().splitlines() if l.strip()]
        cfg=rows[0].get('config',{})
        if cfg.get('implementation')!='onset-consistency-20260915-v1':continue
        for r in rows:
            if r.get('decision')!='accepted':continue
            w={ch:c.wave.filtered(r,ch) for ch in ('BD','BC','AD','AC')}
            picks={ch:c.wave.arrival(x) for ch,x in w.items()}
            for n in c.NAMES:
                s=stats[n];s['original_accepted_frames']+=1
                a=c.row_for(r,w,picks,cfg,n)
                if 'gates' not in a:s['early_invalid']+=1;continue
                failures=[]
                for k,ok in {'D':cfg['D_min']<=a['D']<=cfg['D_max'],'G':cfg['G_min']<=a['G']<=cfg['G_max'],
                     'corr_A':a['A']['corr']>=cfg['frame_corr_A'],'corr_B':a['B']['corr']>=cfg['frame_corr_B'],
                     'boundary':a['B']['lag']<260,'direction_diff':-2<=a['D']<=18}.items():
                    if not ok:s[k+'_failed']+=1;failures.append(k)
                if not failures:s['all_prechecks_pass']+=1
                if failures and n not in examples:
                    examples[n]={'file':p.name,'sequence':r['sequence'],'failed':failures,
                                 'before':{k:r[k] for k in ('A','B','D','G','sos_patient')},
                                 'after':{k:a[k] for k in ('A','B','D','G','sos_patient')}}
    out={'scope':'Overlapping precheck failures on the same751 original accepted frames. Does not evaluate stability or imply truth. Full state replay is separate.', 'counts':stats,'examples':examples}
    (c.HERE/'candidate-gates.json').write_text(json.dumps(out,ensure_ascii=False,indent=2),encoding='utf-8')
    print(json.dumps(stats,indent=2))

if __name__=='__main__':main()
