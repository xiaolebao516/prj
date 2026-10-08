# SOS reference for T/Z and the adult age-SOS chart (2026-10-09)

## Decision

The user chose a Chinese-population reference ("产品主要给中国人用，用中国人的数据集"). Adopted source:

《中国成年居民超声骨速及骨质疏松症分析》，中国公共卫生，2015, 31(4)，表1 — 14 229 urban adults, distal 1/3 radius, Sunlight ultrasound system, mean ± SD by sex and 5-year age group. <https://html.rhhz.net/ZGGGWS/html/20150412.htm>

| Use | Rule |
| --- | --- |
| T-score | (SOS − peak group mean) / peak group SD; female 45~ 4176.9±144.1, male 40~ 4112.8±131.3 |
| Z-score | (SOS − same-sex mean at age) / SD at age; age = whole years + 0.5, linear between group midpoints, 75~ covers all older ages |
| Classification | unchanged: T ≥ −1 正常, −2.5 < T < −1 不足, T ≤ −2.5 严重不足 (ultrasound screening reference, not a DXA diagnosis) |
| Under 20 / sex missing / birth date invalid | SOS only; T, Z, fracture risk and bone age left empty (shown as --); strength 不评定; diagnosis states the reason |

Children: ISCD pediatric positions say T-scores are not used under 20 (peak bone mass not reached). With the old adult reference a typical 10-year-old girl (≈3713 m/s on the group's pediatric chart) would read T ≈ −4.2. No Chinese pediatric radius table was found, so no child Z-score is produced yet.

## Why the old adult bitmaps were replaced

- The old woman/man bitmaps had their own T axes (female T=0 at 4188, 114 per T; male T=0 at 4137, 115 per T), while the code used female 4137 / male 4150 / SD 115.2, so the point on the chart and the T value in the result panel disagreed (female SOS 4074 sat on the T=−1 line but showed T −0.55).
- Digitising the bitmaps: the female mean curve matches the 2015 table within 26 m/s from 25 to 79 years (39 m/s at 20–24); the male curve matches within 11 m/s before 40 and runs 30–70 m/s lower afterwards (`old-chart-vs-2015-table.png`, black dots = table means). The bitmap source was never recorded in the repository (spec: "课题组提供").
- The adult chart is now drawn by `AgeSosChartWidget` from `SosReference`, the same table used for T/Z. Layout proportions, band colours and threshold lines match the old bitmaps; the ±1SD band past the 75~ midpoint is dashed because that group covers everyone older. The two pediatric bitmaps are unchanged.

## Code

- `include/sosreference.h`, `src/sosreference.cpp`: the table, T/Z, age coverage, source label.
- `src/mainwindow.cpp` (`finishAllPatientRounds`): T/Z from `SosReference`; children and incomplete profiles record SOS only.
- `src/agesoschartwidget.cpp`: adult chart drawn from the table; points placed at age + 0.5, the same age the Z-score uses; unused bitmap-era members removed.
- `resources/images/age_sos_woman.bmp`, `age_sos_man.bmp` removed from the qrc and the tree.
- `BoneHealth::deriveResult` produces T/Z, strength, diagnosis, fracture risk and bone age for new results; the same function re-derives saved records for the result card, history dialog and report (user approved 2026-10-09). Stored XML values are not rewritten.
- Bone age (user: "按原来的逻辑" and "不出现不一致"): same nearest-mean rule, now read on the chart's own mean curve at age + 0.5, from the peak onward (male sub-2 m/s bumps flattened). The old placeholder curve `calcAgeReferenceMean` is removed. Woman 36 y, 4000 m/s now reads 59, where the drawn curve reaches 4000 (previously 24).
- Children's chart: drawn from the group's pediatric curves digitised every half year (mean and half the ±1SD band), with no T axis and no green/yellow/red bands, because under-20 results are 不评定. The two pediatric bitmaps are removed.
- Not changed: `classifyBoneStrength`, `calcRelativeFractureRisk` (1.5^−T).

## Checks

| Check | Result |
| --- | --- |
| Main-window safety suite (offscreen, Qt 6.5.3 / MinGW 11.2) | 86 passed, 0 failed, 1 skipped (`onsetGuardMatchesRecordedBatch` needs replay paths); captures enabled |
| New cases | `sosReferenceMatchesPublishedTable` (peak groups, midpoints, interpolation, 75+ clamp, T/Z algebra, under-20 and unknown-sex NaN); `patientResultUsesReferenceAndSkipsChildren` (adult woman T/Z/strength from the table; 10-year-old boy SOS only with `--` in the panel; missing sex); `adultAgeSosChartIsDrawnFromReference` (profiles, visible range exactly T −5…+3, band colours at fixed pixels); `adultChartPointsMatchComputedScores` (on the rendered image, a T = −1 result sits on the T = −1 line for both sexes and a Z = 0 result sits on the mean curve on its steepest stretch, within 1.5 px; with points placed at whole years instead of age + 0.5 the Z check fails by 1.9 px); `savedRecordsAreShownWithCurrentReference` (`deriveResult` adult/child/missing data; a record saved as T −0.85 正常 shows T −1.23 不足 in the result card and the report, free-text diagnosis kept, stored XML values unchanged); `boneAgeAndChildChartStayConsistent` (female SOS equal to the curve at age + 0.5 reads back that age for 47–77; above peak → 47/42; lower SOS never gives a younger bone age for either sex, checked every 1 m/s from 4400 to 3500; children's charts contain no verdict-band pixels and still draw the point) |
| Debug build (`build-debug.ps1`, installed Qt paths) | passed; `BoneDensity.exe` SHA-256 `253E229670C8E24494093A3071C98898FCBE37A096DAA949F357661A6831C377` |
| `git diff --check` | clean |

## Screenshots

- `age-sos-woman.png`, `age-sos-man.png` – new adult charts with anonymous sample points
- `age-sos-girl.png` – children's chart drawn from the digitised curves, neutral background
- `main-1366.png` – main window: the sample record stored as T −0.85 正常 is shown with the current reference as T −1.23 不足 (bone age 59, read on the drawn mean curve)
- `report.png` – report with the new chart; layout unchanged
- `old-chart-vs-2015-table.png` – old bitmaps with the 2015 table means overlaid

## Open

1. Done: saved records are shown with the current reference.
2. Done: bone age keeps the original nearest-mean rule on the chart's curve. Fracture risk (1.5^−T) still has no source (user: leave for now).
3. Pediatric Z-scores need a Chinese pediatric radius reference (user: leave for now).
4. The thesis reports 3800–3850 m/s for a subject on both the commercial device and this system; with this reference that is T ≈ −2.0 to −2.6 depending on sex. Agreement with the population database's device (site, probe spacing) should be checked before clinical use.
