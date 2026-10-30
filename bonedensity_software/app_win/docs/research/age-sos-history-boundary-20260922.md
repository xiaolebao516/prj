# Age-SOS longitudinal chart: child/adult boundary decision

## Decision question

When one patient has measurements on both sides of age 20, should child and adult points be drawn together on the same age-SOS reference image?

## Repository facts

- `AgeSosChartWidget::profileFor` selects Girl/Boy for age `<20` and Woman/Man for age `>=20`.
- The child and adult bitmaps use different age ranges, SOS ranges, plot coordinates, and reference curves.
- Measurement records already carry measurement-time age, date, sex, birth-date snapshot, and SOS. No schema migration is needed to classify historical records.

## External evidence

- ISCD pediatric positions treat pediatric assessment and serial reporting as a distinct reporting domain, require age/sex-appropriate reference data, and define pediatric fracture-history criteria through age 19: https://iscd.org/wp-content/uploads/2024/03/2019-ISCD-Pediatric-Postions.pdf
- ISCD adult positions separately define adult reference/reporting and serial-comparison rules: https://iscd.org/official-positions-2023/
- FDA bone-sonometer guidance states that ultrasound reference databases are device/reference-population dependent and that age-matched Z-scores and young-adult T-scores have different reference meanings: https://www.fda.gov/medical-devices/guidance-documents-medical-devices-and-radiation-emitting-products/bone-sonometers-class-ii-special-controls-guidance-industry-and-fda-staff

These sources do not prescribe this application's UI. They support the engineering inference that child and adult points should not be plotted against one shared reference image when the underlying reference databases and axes differ.

## Recommended product rule

1. Select the chart profile from the focal measurement: latest valid measurement on the main page; the report's own measurement on a report.
2. Plot only valid historical records belonging to the same child/adult profile and sex.
3. On an adult chart, omit `<20` points and show a short note such as `另有 2 条儿童期记录未在本图显示` when applicable. Keep those records in the archive/history table.
4. On a child chart, omit adult records. This mostly matters when an old child report is reopened after the patient becomes an adult.
5. A report includes same-profile records no later than the report measurement date. It never adds future measurements to an older report.
6. Do not force child and adult records into one coordinate system or add a second chart to the one-page report unless the user later requests a dedicated longitudinal report.

## Status

Recommended and awaiting user confirmation. The report layout and adjacent history list are already approved.
