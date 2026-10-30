"""Summarize the pose-independent stability counterfactual without editing inputs."""
import json
import pathlib
import statistics
import sys


def summarize(files, key):
    completion_gains = []
    first_gains = []
    sos_deltas = []
    changed = []
    newly_accepted = []
    baseline_discards = 0
    candidate_discards = 0
    baseline_complete = 0
    candidate_complete = 0
    for entry in files:
        baseline = entry["baseline"]
        candidate = entry[key]
        baseline_discards += baseline["discarded_values"]
        candidate_discards += candidate["discarded_values"]
        baseline_complete += bool(baseline["reached_30"])
        candidate_complete += bool(candidate["reached_30"])
        newly_accepted.extend(candidate["newly_accepted_sos"])
        row = {"file": entry["file"], "damaged_prefix_only": entry["damaged_prefix_only"]}
        different = baseline["reached_30"] != candidate["reached_30"]
        if baseline["reached_30"] and candidate["reached_30"]:
            completion_gain = (baseline["completion_ms_on_recorded_timeline"]
                               - candidate["completion_ms_on_recorded_timeline"])
            completion_gains.append(completion_gain)
            row["completion_gain_ms"] = completion_gain
            delta = candidate["round_summary"]["sos"] - baseline["round_summary"]["sos"]
            sos_deltas.append(delta)
            row["sos_delta"] = delta
            different |= completion_gain != 0 or abs(delta) > 1e-9
        if baseline["first_accepted_ms"] >= 0 and candidate["first_accepted_ms"] >= 0:
            first_gain = baseline["first_accepted_ms"] - candidate["first_accepted_ms"]
            first_gains.append(first_gain)
            row["first_gain_ms"] = first_gain
            different |= first_gain != 0
        if baseline["discarded_values"] != candidate["discarded_values"]:
            row["discard_delta"] = candidate["discarded_values"] - baseline["discarded_values"]
            different = True
        if different:
            row["baseline_complete"] = baseline["reached_30"]
            row["candidate_complete"] = candidate["reached_30"]
            changed.append(row)
    return {
        "baseline_completed": baseline_complete,
        "candidate_completed": candidate_complete,
        "baseline_discarded": baseline_discards,
        "candidate_discarded": candidate_discards,
        "changed_count": len(changed),
        "completion_earlier": sum(x > 0 for x in completion_gains),
        "completion_later": sum(x < 0 for x in completion_gains),
        "completion_gain_ms_max": max(completion_gains, default=None),
        "completion_loss_ms_max": -min(completion_gains, default=0),
        "first_earlier": sum(x > 0 for x in first_gains),
        "first_later": sum(x < 0 for x in first_gains),
        "first_gain_ms_max": max(first_gains, default=None),
        "first_gain_ms_median_positive": statistics.median([x for x in first_gains if x > 0]) if any(x > 0 for x in first_gains) else None,
        "sos_delta_abs_max": max((abs(x) for x in sos_deltas), default=None),
        "sos_delta_range": [min(sos_deltas, default=None), max(sos_deltas, default=None)],
        "newly_accepted_count": len(newly_accepted),
        "newly_accepted_sos_range": [min(newly_accepted, default=None), max(newly_accepted, default=None)],
        "changed": changed,
    }


data = json.loads(pathlib.Path(sys.argv[1]).read_text(encoding="utf-8"))
keys = sys.argv[2:] or ("pose_only", "pose_and_relock", "d_grace_2",
                        "d_grace_5", "d_grace_10", "d_grace_20")
print(json.dumps({key: summarize(data["files"], key)
                  for key in keys if key in data["files"][0]}, indent=2))
