# Calibration Domain Reference

> **Authority:** Agent-only, task-triggered operational conclusions for calibration work. Product behavior is normative in [REQ-009](../product-spec.md#req-009标准试块探头校准); sources and derivation are in the human [calibration research](../../../docs/research/probe-calibration-research.md).

## Settled Operational Conclusions

- The project uses `±2%` SOS error and `≤1%` repeatability as engineering verification thresholds informed by YY/T 0774-2019 product-performance clauses; they are not a standardized internal-D adjustment method.
- YY/T requires at least six measurements for repeatability. The project's 6 calibration + re-coupling + 6 independent validation sequence is a project design, not a direct standard mandate.
- Candidate calculation remains `candidate D = current D × reference SOS / measured B-channel mean SOS`, assuming the same sampling period and peak-selection behavior.
- B produces the candidate; A is diagnostic. Candidate state stays isolated until independent verification succeeds and the operator confirms activation.
- Reference SOS must come from a certificate or traceable source at the applicable temperature. Software must not invent a temperature curve.
- One block can support project engineering validation but cannot prove product conformity, for which YY/T describes at least three media with different sound speeds.
- JJF 1649-2017 must not be used as clause-level authority here because no formal full text was obtained.
- Result wording is limited to “工程验证通过/未通过”.

## External Acceptance Boundaries

Software evidence cannot establish certificate authenticity, qualified-block identity, thermometer accuracy, true device/probe temperature, real 6+6 acquisition, coupling quality, re-placement repeatability, peak stability, physical D accuracy, patient-measurement impact, three-medium product conformity, metrological calibration, clinical validity, or field acceptance.

## Prohibited Claims

Do not state that:

- the device is “国家标准/计量校准合格” or fully conforms to YY/T;
- a standard prescribes the internal-D formula or the project's 6+6 workflow;
- `±2%` accuracy has been physically proven without real qualified evidence;
- software verified temperature truth, coupling quality, certificate authenticity, or field acceptance;
- a software-generated temperature curve is traceable.

Keep sources and clause rationale in the human research document; do not duplicate that long-form material here.
