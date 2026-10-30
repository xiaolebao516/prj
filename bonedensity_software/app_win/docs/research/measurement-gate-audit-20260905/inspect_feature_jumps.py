"""Read-only adjacent-frame feature audit; stop at damaged input, no repairs.

Describes logged features, not physical motion or correctness of rejected SOS.
Run with a log directory; outputs JSON, never stores patient identities.
"""
import json
import pathlib
import sys


def inspect(path):
    previous = None
    count = 0
    gate_frames = 0
    formula_errors = 0
    largest = []
    gate_flips = []
    with path.open('rb') as stream:
        header = json.loads(stream.readline())
        for line in iter(lambda: stream.readline(1024 * 1024), b''):
            if b'\0' in line or not line.endswith(b'\n'):
                break
            row = json.loads(line)
            if row['event'] != 'frame':
                continue
            if 'gates' not in row:
                previous = None
                continue
            am = (row['A']['early_feature'] + row['A']['late_feature']) / 2
            bm = (row['B']['early_feature'] + row['B']['late_feature']) / 2
            gate_frames += 1
            formula_errors += abs(bm - am - row['G']) > 1e-9
            current = dict(sequence=row['sequence'], ms=row['elapsed_ms'],
                G=row['G'], D=row['D'], A_mid=am, B_mid=bm,
                A_early=row['A']['early_feature'], A_lag=row['A']['lag'],
                B_lag=row['B']['lag'], SOS=row['sos_patient'],
                branch=row.get('A_feature_branch'), decision=row['decision'],
                gates=row['gates'])
            if (previous and 0 < current['ms'] - previous['ms'] <= 200
                    and current['B_lag'] == previous['B_lag']
                    and abs(current['B_mid'] - previous['B_mid']) <= 2
                    and abs(current['G'] - previous['G']) >= 20
                    and abs(current['A_mid'] - previous['A_mid']) >= 20):
                count += 1
                pair = [previous, current]
                largest.append(pair)
                largest.sort(key=lambda p: abs(p[1]['G'] - p[0]['G']), reverse=True)
                largest = largest[:2]
                if (current['gates']['G'] != previous['gates']['G']
                        and current['gates']['D'] and previous['gates']['D']
                        and len(gate_flips) < 3):
                    gate_flips.append(pair)
            previous = current
    return dict(file=path.name, utc=header['utc'], round=header['config']['round'],
                gate_frames=gate_frames, formula_errors=formula_errors,
                jump_count=count, largest_examples=largest,
                first_G_flip_examples_with_D_pass=gate_flips)


if __name__ == '__main__':
    results = [inspect(p) for p in sorted(pathlib.Path(sys.argv[1]).glob('round-*.jsonl'))]
    print(json.dumps(results, ensure_ascii=False))
