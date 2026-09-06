"""Read-only inspection of a no-progress experiment; not waveform replay or tuning.

Usage: python inspect_no_progress.py path/to/round-*.jsonl
Prints aggregate diagnostics only; never rewrites input or patient files.
"""
import collections
import hashlib
import json
import pathlib
import statistics
import sys


def inspect(path):
    data = path.read_bytes()
    rows = [json.loads(line) for line in data.decode("utf-8").splitlines() if line.strip()]
    assert rows[0]["event"] == "start" and rows[-1]["event"] == "stop", "Incomplete recording"
    assert [r["sequence"] for r in rows] == list(range(1, len(rows) + 1)), "Sequence gap"
    config = rows[0]["config"]
    assert config["implementation"] == "observe-before-g-20260906-v1", "Unreviewed flow profile"
    frames = [r for r in rows if r["event"] == "frame"]
    assert not any(r.get("decision") == "accepted" or r["locked"] for r in frames), "Use full replay for accepted/locked records"
    gates = [r for r in frames if "gates" in r]
    prechecks = ("AB_diff", "B_jump", "D", "G", "boundary", "corr_A", "corr_B", "direction")
    for r in gates:
        g = r["gates"]
        assert g["all_prechecks_passed"] == all(g[k] for k in prechecks)
        assert g["stability_evaluated"] == all(g[k] for k in prechecks if k != "G")
        assert g["corr_A"] == (r["A"]["corr"] >= config["frame_corr_A"])
    only_a = [r for r in gates if not r["gates"]["corr_A"] and
              all(r["gates"][k] for k in prechecks if k != "corr_A")]
    dg = [r for r in gates if r["gates"]["D"] and r["gates"]["G"]]

    def max_warmup(field):
        # Valid only before first lock; compare observation opportunities, not SOS.
        count = rejects = peak = 0
        for r in frames:
            if r.get("gates", {}).get(field, False):
                count += 1
                rejects = 0
                peak = max(peak, count)
            else:
                rejects += 1
                if rejects >= config["unlock_count"]:
                    count = rejects = 0
        assert peak < config["warmup"], "Requires full stability replay"
        return peak

    intervals = [b["elapsed_ms"] - a["elapsed_ms"] for a, b in zip(frames, frames[1:])]
    return {
        "file": path.name, "sha256": hashlib.sha256(data).hexdigest(),
        "profile": config["implementation"], "duration_ms": rows[-1]["elapsed_ms"],
        "frames": len(frames), "decisions": dict(collections.Counter(r["decision"] for r in frames)),
        "gate_frames": len(gates),
        "overlapping_gate_failures": dict(collections.Counter(k for r in gates for k in prechecks if not r["gates"][k])),
        "DG_pass": len(dg), "DG_pass_A_fail": sum(not r["gates"]["corr_A"] for r in dg),
        "only_A_fail": len(only_a),
        "only_A_fail_corr_median": statistics.median(r["A"]["corr"] for r in only_a) if only_a else None,
        "only_A_fail_at_least_075": sum(r["A"]["corr"] >= .75 for r in only_a),
        "all_prechecks_pass": sum(r["gates"]["all_prechecks_passed"] for r in gates),
        "stability_observations": sum(r["gates"]["stability_evaluated"] for r in gates),
        "max_trial_warmup": max_warmup("stability_evaluated"),
        "max_default_warmup_same_features": max_warmup("all_prechecks_passed"),
        "warmup_required": config["warmup"],
        "discard_events": sum(r["event"] == "discard_partial" for r in rows),
        "discarded_effective_values": sum(r.get("discarded_values", 0) for r in rows),
        "frame_interval_median_ms": statistics.median(intervals), "frame_interval_max_ms": max(intervals),
        "processing_median_ms": statistics.median(r["processing_ms"] for r in frames),
        "processing_max_ms": max(r["processing_ms"] for r in frames),
        "scope": "Recorded decisions and pre-lock observation counting only; no raw waveform replay or accuracy conclusion."
    }


if __name__ == "__main__":
    print(json.dumps(inspect(pathlib.Path(sys.argv[1])), ensure_ascii=False, indent=2))
