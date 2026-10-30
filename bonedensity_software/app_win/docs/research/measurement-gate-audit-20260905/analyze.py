"""Read-only audit of historical angle-feature CSVs; no patient algorithm changes.

Usage: bundled-python analyze.py --input-root <attachment F directory>
Historical rows are already selected by their recording code. Replay is a
conditional counterfactual on recorded features, not end-to-end device replay.
"""
import argparse
import hashlib
import json
from pathlib import Path

import numpy as np
import pandas as pd


def distribution(values):
    v = np.asarray(values, dtype=float)
    v = v[np.isfinite(v)]
    if not len(v):
        return None
    return {"n": len(v), "min": float(v.min()), "p10": float(np.quantile(v, .1)),
            "median": float(np.median(v)), "p90": float(np.quantile(v, .9)),
            "max": float(v.max()), "mean": float(v.mean()),
            "sd": float(v.std(ddof=1)) if len(v) > 1 else 0}


def masks(d, dmin=5, dmax=15, gmax=6, amin=.78, bmin=.55):
    return {
        "B_jump": (d.lagB - (d.bcOnset - d.bdOnset)).abs() <= 70,
        "boundary": d.lagB < 260,
        "AB_abs_diff": (d.lagA - d.lagB).abs() <= 18,
        "direction": d.lagA - d.lagB >= -2,
        "corrA": d.corrA >= amin,
        "corrB": d.corrB >= bmin,
        "D": (d.lagA - d.lagB).between(dmin, dmax),
        "G": d.pairMidGap.abs() <= gmax,
    }


def replay(d, mask, warmup=14, need=10, tolerance=5):
    """Port checkBoneLagStable and 30-frame round accumulation; no user delays.

    Resets round-local state after each 30 accepts as startPatientMeasurement
    does. Includes round correlation gates, not five-round final clustering.
    Assumes recorded B/A pair results are otherwise valid.
    """
    recent, accepted, rounds = [], [], []
    locked, center, outside, start = False, 0, 0, 0
    first_valid = None
    count_valid = 0
    times = pd.to_datetime(d.time)
    for pos, row in enumerate(d.itertuples(index=False)):
        if not mask.iloc[pos]:
            continue
        lag = int(row.lagB)
        recent.append(lag)
        recent = recent[-20:]
        valid = False
        if not locked:
            if len(recent) < warmup:
                continue
            candidate = sorted(recent)[len(recent) // 2]
            if sum(abs(v-candidate) <= tolerance for v in recent) < need:
                continue
            locked, center, outside = True, candidate, 0
            valid = abs(lag-center) <= tolerance
        elif abs(lag-center) <= tolerance:
            valid, outside = True, 0
        else:
            outside += 1
            if outside >= 10:
                recent, locked, center, outside = [], False, 0, 0
        if not valid:
            continue
        count_valid += 1
        if first_valid is None:
            first_valid = float((times.iloc[pos]-times.iloc[0]).total_seconds())
        accepted.append((float(row.sosB), float(row.corrA), float(row.corrB), pos))
        if len(accepted) == 30:
            trimmed = np.sort(np.array(accepted)[:, :3], axis=0)[6:24].mean(axis=0)
            ok = bool(trimmed[1] >= .80 and trimmed[2] >= .55)
            rounds.append({"sos": float(trimmed[0]), "corrA": float(trimmed[1]),
                           "corrB": float(trimmed[2]), "quality_pass": ok,
                           "first_csv_row": accepted[0][3]+2, "last_csv_row": pos+2,
                           "duration_s": float((times.iloc[pos]-times.iloc[start]).total_seconds()),
                           "accepted_sos_span": max(a[0] for a in accepted)-min(a[0] for a in accepted)})
            recent, accepted = [], []
            locked, center, outside, start = False, 0, 0, pos+1
    return {"valid_count": count_valid, "first_valid_s": first_valid,
            "rounds": rounds, "partial_valid_count": len(accepted)}


def audit(path):
    d = pd.read_csv(path)
    for col in d.columns.difference(["time", "mode"]):
        d[col] = pd.to_numeric(d[col], errors="raise")
    times = pd.to_datetime(d.time)
    tests = masks(d)
    all_ok = pd.concat(tests, axis=1).all(axis=1)
    dg = tests["D"] & tests["G"]
    summary = {
        "filename": path.name, "rows": len(d), "columns": len(d.columns),
        "mode_counts": d["mode"].value_counts().to_dict(),
        "missing_cells": int(d.isna().sum().sum()),
        "time_monotonic": bool(times.is_monotonic_increasing),
        "duration_s": float((times.iloc[-1]-times.iloc[0]).total_seconds()),
        "interval_s": distribution(times.diff().dt.total_seconds()),
        "gaps_over_2s": int((times.diff().dt.total_seconds() > 2).sum()),
        "features": {k: distribution(d[k]) for k in ["sosAvg", "sosA", "sosB", "lagA", "lagB", "signedLagDiff", "pairMidGap", "corrA", "corrB"]},
        "invariants": {
            "D_matches_lag_difference": bool(np.allclose(d.signedLagDiff, d.lagA-d.lagB)),
            "G_matches_pair_mid_difference": bool(np.allclose(d.pairMidGap, d.pairMidB-d.pairMidA)),
            "B_K_sos_times_lag": distribution(d.sosB*d.lagB),
            "A_K_sos_times_lag": distribution(d.sosA*d.lagA),
            "sosAvg_equals_B_count": int(np.isclose(d.sosAvg, d.sosB, atol=.02, rtol=0).sum()),
            "sosAvg_equals_weighted_count": int(np.isclose(d.sosAvg, d.wB*d.sosB+(1-d.wB)*d.sosA, atol=.02, rtol=0).sum()),
            "ac_onset_missing": int((d.acOnset < 0).sum()),
            "B_mid_matches_envelope_onsets": int(np.isclose(d.pairMidB, (d.bcOnset+d.bdOnset)/2).sum()),
            "A_mid_matches_envelope_onsets": int(np.isclose(d.pairMidA, (d.acOnset+d.adOnset)/2).sum()),
            "A_mid_matches_debug_valleys": int(np.isclose(d.pairMidA, (d.acValley+d.adValley)/2).sum()),
        },
        "gate_individual_pass": {k: int(v.sum()) for k, v in tests.items()},
        "DG_pass": int(dg.sum()), "all_prechecks_pass": int(all_ok.sum()),
        "document_G4_pass": int((d.pairMidGap.abs() <= 4).sum()),
        "near_centers_D8to10_G1_count": int(((d.lagA-d.lagB).between(8,10) & (d.pairMidGap.abs()<=1)).sum()),
        "near_centers_but_precheck_fail": int(((d.lagA-d.lagB).between(8,10) & (d.pairMidGap.abs()<=1) & ~all_ok).sum()),
        "DG_pass_but_quality_fail": int((dg & ~all_ok).sum()),
        "precheck_pass_sosB": distribution(d.loc[all_ok, "sosB"]),
        "one_gate_removed_pass": {k: int(pd.concat({a:b for a,b in tests.items() if a!=k}, axis=1).all(axis=1).sum()) for k in tests},
        "scenarios": {},
    }
    scenarios = {"current": {}, "G8": {"gmax":8}, "G10": {"gmax":10},
                 "G15": {"gmax":15}, "D0to18_G6": {"dmin":0,"dmax":18},
                 "A070": {"amin":.70}, "no_DG": {"dmin":-1e9,"dmax":1e9,"gmax":1e9}}
    homogeneous = d["mode"].nunique() == 1 and bool(np.isclose(d.sosAvg,d.sosB,atol=.02,rtol=0).all())
    summary["conditional_replay_eligible"] = homogeneous
    summary["replay_limit"] = ("Recorded features only; missing rejected frames and start/stop markers; timestamps do not measure uninterrupted acquisition."
        if homogeneous else "Mixed modes/output conventions; no continuous replay attempted.")
    for name, args in scenarios.items():
        passed = pd.concat(masks(d, **args), axis=1).all(axis=1)
        summary["scenarios"][name] = {
            "prechecks_pass": int(passed.sum()),
            "selected_sosB": distribution(d.loc[passed, "sosB"]),
            "conditional_replay": replay(d, passed) if homogeneous else None,
        }
    return summary


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input-root", type=Path, required=True)
    parser.add_argument("--output", type=Path)
    args = parser.parse_args()
    seen, duplicates, result = {}, [], []
    for path in sorted(args.input_root.rglob("*.csv")):
        digest = hashlib.sha256(path.read_bytes()).hexdigest()
        if digest in seen:
            duplicates.append({"duplicate": str(path.relative_to(args.input_root)), "same_as": seen[digest], "sha256": digest})
            continue
        seen[digest] = str(path.relative_to(args.input_root))
        item = audit(path)
        item.update({"relative_source": seen[digest], "sha256": digest})
        result.append(item)
    output = {"scope": "Conditional feature replay, not raw-waveform or clinical validation", "duplicates": duplicates, "files": result}
    serialized = json.dumps(output, ensure_ascii=False, indent=2)
    if args.output:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(serialized, encoding="utf-8")
    else:
        print(serialized)


if __name__ == "__main__":
    main()
