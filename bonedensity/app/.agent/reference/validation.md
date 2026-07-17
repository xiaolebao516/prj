# Validation Reference

> **Authority:** Agent-only, task-triggered map of durable verification evidence and external-acceptance boundaries. It does not record current task state or replace an active Loop verification plan. Normative requirements are in [product-spec.md](product-spec.md).

## Evidence Classification

Always distinguish:

1. actual software execution;
2. code-path inspection;
3. supervised device, printer, or laboratory acceptance.

A build proves compilation/deployment only. The calibration suite proves only its focused software scope.

## Reproduction Entry Points

- `powershell -ExecutionPolicy Bypass -File ./build-debug.ps1`: canonical Qt 6.5.3 / MinGW 11.2 clean Debug build.
- `powershell -ExecutionPolicy Bypass -File ./test-calibration.ps1`: calibration formula, boundary, candidate-isolation, permission, persistence, recovery, and UI checks.
- `testdata/age-sos-reference/`: anonymous four-population, age-boundary, newest-record, legacy-record, empty, and out-of-range demonstrations.
- [probe-calibration-research.md](../../docs/research/probe-calibration-research.md): formal-source and formula rationale for calibration evidence.
- `docs/evidence/<date>-<feature>/`: only a minimal reproducible set of approved visual evidence when needed.

## Existing Software Evidence

- Account corruption recovery, normal-account preservation, and `admin / 1` authentication have actual-run evidence.
- Patient/history XML save, reload, legacy migration, and historical snapshot cases have actual-run evidence.
- Default responsive layout was user-confirmed.
- New/legacy reports, archive entry, A4 PDF rendering, and the system print dialog have software evidence.
- Age-SOS population backgrounds, 19/20 boundary, newest result, legacy age calculation, empty/out-of-range states, and default/maximized layouts have software-demonstration evidence.
- Calibration formula, 30-frame/6-measurement levels, thresholds, candidate isolation, permissions, XML recovery/rollback, six-step flow, and default-size layout are covered by focused tests and isolated execution.
- The fixed Qt/MinGW Debug build completes and the executable starts without the mixed-runtime entry-point failure.

These records do not replace the external acceptance list in the product specification.

## External Boundary

Hardware acquisition, the real five-round flow, USB hot-unplug/reconnection, physical print output, coupling repeatability, temperature truth, qualified-phantom accuracy, certificate authenticity, and patient impact remain external unless actually exercised with reproducible evidence.
