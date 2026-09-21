"""Feature/state replica validated against original decisions before candidate testing.

The only candidate change is to reject a frame before stability observation when
B-channel onset follows its first threshold hit by more than the candidate limit.
Recorded features are unchanged; no new wave is chosen, no SOS target is used.
No Qt GUI, serial, inter-round scheduling or storage behavior is simulated.
"""
from pathlib import Path
import json
import sys

ROOT=Path(__file__).resolve().parents[3]
WORK=ROOT/'build/wave-audit-20260915'

class State:
    def __init__(self,c):
        self.c=c;self.values=[];self.reset()
    def reset(self):
        self.rejected=0;self.recent=[];self.locked=False;self.center=0
        self.outside=0;self.pending=False;self.previous=0
    def reject(self):
        self.rejected+=1
        if self.rejected>=self.c['unlock_count']:
            defer=bool(self.values) and (self.locked or self.pending)
            prev=self.center if self.locked else self.previous
            self.reset()
            if defer:
                self.pending=True;self.previous=prev
            else:
                self.values=[]
    def stable(self,lag):
        self.rejected=0;self.recent.append(lag)
        self.recent=self.recent[-self.c['window_size']:]
        tol=self.c['lag_tolerance']
        if not self.locked:
            if len(self.recent)<self.c['warmup']:
                return False
            center=sorted(self.recent)[len(self.recent)//2]
            if sum(abs(v-center)<=tol for v in self.recent)<self.c['lock_need']:
                return False
            self.locked=True;self.center=center;self.outside=0
            if self.pending:
                same=abs(center-self.previous)<=self.c['partial_relock_retention_lag']
                same=same and all(abs(r['B']['lag']-center)<=self.c['partial_relock_retention_lag'] for r in self.values)
                self.pending=False;self.previous=0
                if not same:
                    self.values=[]
            return abs(lag-self.center)<=tol
        if abs(lag-self.center)<=tol:
            self.outside=0
            return True
        self.outside+=1
        if self.outside>=self.c['unlock_count']:
            self.reset();self.values=[]
        return False

def trimmed(v):
    v=sorted(v);k=int(len(v)*.2) if len(v)>=10 else 0
    v=v[k:len(v)-k]
    return sum(v)/len(v)

def run(f,limit=None):
    c=f['config'];w=State(c);accepted=[];summary=None;completion=None;rejected_by_candidate=0
    for r in f['frames']:
        bad=limit is not None and any(r['picks'][ch] is not None and r['picks'][ch]['onset']-r['picks'][ch]['hit']>limit for ch in ['BD','BC'])
        rejected_by_candidate+=int(bad)
        counted=False
        if bad or 'gates' not in r:
            w.reject()
        else:
            b,a=r['B'],r['A'];d=a['lag']-b['lag'];g=r['G']
            base=(abs(b['lag']-b['rough_lag'])<=70 and b['lag']<260 and abs(d)<=18 and d>=-2
                  and a['corr']>=c['frame_corr_A'] and b['corr']>=c['frame_corr_B']
                  and c['D_min']<=d<=c['D_max'])
            stable=w.stable(b['lag']) if base else False
            if not base:
                w.reject()
            counted=base and c['G_min']<=g<=c['G_max'] and stable
            if limit is None:
                assert base==r['gates']['stability_evaluated'],(f['file'],r['sequence'],'prechecks')
        if limit is None:
            assert counted==(r['decision']=='accepted'),(f['file'],r['sequence'],'accepted')
            assert (w.locked,w.center,len(w.values))==(r['locked'],r['locked_lag'],r['partial_values_before_accept']),(f['file'],r['sequence'],'state')
        if counted:
            accepted.append(dict(sequence=r['sequence'],sos=r['sos_patient']))
            w.values.append(r)
            if len(w.values)>=c['frame_target']:
                summary={k:trimmed([get(v) for v in w.values]) for k,get in {
                    'sos':lambda r:r['sos_patient'],'corr_A':lambda r:r['A']['corr'],
                    'corr_B':lambda r:r['B']['corr'],'D':lambda r:r['D'],'G':lambda r:r['G']}.items()}
                summary['quality_pass']=(summary['corr_A']>=c['round_corr_A'] and summary['corr_B']>=c['round_corr_B']
                    and c['D_min']<=summary['D']<=c['D_max'] and c['G_min']<=summary['G']<=c['G_max'])
                completion=r['elapsed_ms'];break
    if limit is None:
        expected=next((r for r in f['events'] if r['event']=='round_summary'),None)
        assert (summary is None)==(expected is None),(f['file'],'summary presence')
        if summary:
            assert all(abs(v-expected[k])<1e-8 for k,v in summary.items()),(f['file'],'summary values')
            # The summary is logged 1-3 ms after its triggering frame in this batch.
            # Verify the exact triggering sequence; compare candidates on frame time.
            assert r['sequence']+1==expected['sequence'],(f['file'],'summary trigger')
            assert completion<=expected['elapsed_ms'],(f['file'],'summary ordering')
    return dict(file=f['file'],accepted=accepted,summary=summary,completion_ms=completion,
                candidate_rejections=rejected_by_candidate,partial_remaining=len(w.values) if summary is None else 0)

def main():
    global WORK
    date=sys.argv[1] if len(sys.argv)>1 else '20260915'
    assert date in ('20260914','20260915')
    WORK=ROOT/('build/wave-audit-'+date)
    x=json.loads((WORK/'results.json').read_text(encoding='utf-8'))
    baseline=[run(f) for f in x['files']]
    scenarios={}
    for limit in [30,40,60,80]:
        cases=[run(f,limit) for f in x['files']]
        changes=[]
        for b,c in zip(baseline,cases):
            if (b['summary'],b['completion_ms'])!=(c['summary'],c['completion_ms']):
                changes.append(dict(file=b['file'],before=b['summary'],after=c['summary'],before_ms=b['completion_ms'],after_ms=c['completion_ms']))
        scenarios[str(limit)]=dict(cases=cases,changed_rounds=changes)
    output=dict(scope='Validated feature/stability/30-value-round replica; conditional recorded-trajectory candidate, not a Qt execution or prospective trial.',
        baseline=baseline,scenarios=scenarios)
    (WORK/'state-replay.json').write_text(json.dumps(output,indent=2),encoding='utf-8')
    print('BASELINE matched all',len(baseline),'files')
    for limit,s in scenarios.items():
        print('LIMIT',limit,'changed rounds',len(s['changed_rounds']),'accepted',sum(len(c['accepted']) for c in s['cases']),
              'low',sum(r['sos']<3000 for c in s['cases'] for r in c['accepted']))
        print(json.dumps(s['changed_rounds']))

if __name__=='__main__':
    main()
