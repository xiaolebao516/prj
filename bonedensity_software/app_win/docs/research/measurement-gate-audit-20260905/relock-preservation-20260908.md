# Stable-cluster relock repair — 2026-09-08

## Decision

Use one isolated hardware trial for a narrow state-management repair. After sustained precheck loss, do not irreversibly delete already accepted partial-round values until a replacement strict-D-qualified B-lag cluster is established. Retain only when the old center, new center, and the B-derived lag of every saved value are all within two samples. A different cluster is discarded before its current frame can count.

This changes neither SOS nor feature estimation, D/G/correlation ranges, warmup/unlock counts, round rules, acquisition timing, persistence, or the default/G6 profiles.

## Demonstrated defect and repair

The old path deleted partial progress as soon as ten consecutive precheck failures occurred, even though it did not yet know whether the probe had moved to another speed cluster. The focused reproduction stored five values at lag128, triggered expiry, and then relocked at128. Before the repair the five values were already gone. After the repair they remain; relocking at150 deletes all companion lists before the new frame can count.

## Full recorded-trajectory result

The replay used all63 afternoon files from the preserved manifest. Two damaged files were restricted to their independently complete prefixes. The original dual-window/G6 decisions and59 complete summaries reproduced.

| Outcome | Baseline | Tight relock |
| --- | ---: | ---: |
| Complete intact rounds | 59 | 59 |
| Discarded partial values (including validated prefixes) | 263 | 214 |
| Lost or later completed rounds | 0 | 0 |

Three intact rounds completed earlier by0.754s,5.163s, and1.124s. Their round SOS changes were0,-4.65, and-6.30m/s. This is evidence of avoiding some repeated work on fixed trajectories, not a prediction of prospective measurement time.

## Rejected alternatives

- B local-peak completion passed its synthetic boundary case but full raw replay reduced completed rounds from59 to54, never completed earlier, and delayed one first acceptance from60.512s to117.596s.
- Observing B stability before both D and G reduced completed rounds59 to57 and changed one round by225.46m/s. D therefore participates in choosing which B-lag cluster is measured.
- Keeping compatible D-only failures indefinitely, or allowing even two extra frames before expiry, retained an early4053.58m/s round where the baseline later discarded that cluster and completed at3828.12m/s. No D grace variant is suitable.
- A loose same-center relock rule (±5) improved more trajectories but also changed a round by225.46m/s. It was replaced by the all-values ±2 rule.

None of these comparisons selected results toward3900.

## Verification and artifact

- Default main-window suite:52 passed,0 failed,1 optional screenshot skip in both Release and Debug.
- Relock-profile suite:52 passed,0 failed,1 optional screenshot skip in both Release and Debug.
- Isolated executable: `build/self-trial-relock-preservation/debug/BoneDensity_RelockTrial.exe`.
- SHA-256: `A7677B95A04CED451D8682340AB7FF1E997F2E41FE1F3765B7F59182AC2A4387`.
- Distinct title and Debug log marker: `relock-preservation-20260908-v1`; retention lag recorded as2.
- Default executable remains `661043A6578CFFB9A54975077B208C5F8C092F2DC4E2FA2F275B66F844A584CE`.
- Previous dual-window/G6 executable remains `00A2196028F7EC82F91DC4A8F8360615959021AD643A8B501089E8C675E2914D`.
- No clinical XML was copied to the new runtime.

## External acceptance

Run repeated measurements on the instrument and compare with the previous dual-window trial. Check whether visible partial progress is reset less often, whether each round completes without new stalls, and whether repeated/final SOS remains acceptably stable and near contemporaneous sample-machine results. The replay cannot establish those physical outcomes.
