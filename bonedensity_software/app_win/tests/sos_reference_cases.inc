// SOS reference (中国公共卫生 2015 表1) cases: T/Z, children, and the adult
// age-SOS chart drawn from the same table.

void MainWindowSafetyTests::sosReferenceMatchesPublishedTable()
{
    using SosReference::Sex;
    const SosReference::Stat female = SosReference::youngAdult(Sex::Female);
    const SosReference::Stat male = SosReference::youngAdult(Sex::Male);
    // Peak groups of Table 1: female 45~, male 40~.
    QCOMPARE(female.mean, 4176.9);
    QCOMPARE(female.sd, 144.1);
    QCOMPARE(male.mean, 4112.8);
    QCOMPARE(male.sd, 131.3);
    QVERIFY(!SosReference::youngAdult(Sex::Unknown).isValid());

    // Group midpoints return the table values; between them it is linear.
    QCOMPARE(SosReference::atAge(Sex::Female, 62.5).mean, 3958.9);
    QCOMPARE(SosReference::atAge(Sex::Male, 22.5).sd, 118.6);
    QVERIFY(qAbs(SosReference::atAge(Sex::Female, 25.0).mean - (4079.9 + 4126.6) / 2) < 1e-9);
    // 75~ is the oldest group and covers everyone older.
    QCOMPARE(SosReference::atAge(Sex::Male, 90.0).mean, 4030.2);
    QCOMPARE(SosReference::atAge(Sex::Female, 20.0).mean, 4079.9);

    QVERIFY(qAbs(SosReference::tScore(4176.9, Sex::Female)) < 1e-9);
    QVERIFY(qAbs(SosReference::tScore(4176.9 - 2.5 * 144.1, Sex::Female) + 2.5) < 1e-9);
    QVERIFY(qAbs(SosReference::tScore(4112.8 - 131.3, Sex::Male) + 1.0) < 1e-9);
    // A 22-year-old is compared with the 20~24 group (midpoint 22.5).
    QVERIFY(qAbs(SosReference::zScore(4079.9 - 111.1, Sex::Female, 22) + 1.0) < 1e-9);

    QVERIFY(!SosReference::coversAge(19));
    QVERIFY(SosReference::coversAge(20));
    QVERIFY(std::isnan(SosReference::zScore(3800.0, Sex::Female, 19)));
    QVERIFY(std::isnan(SosReference::tScore(3800.0, Sex::Unknown)));
    QCOMPARE(SosReference::sexFromGender(QStringLiteral("女")), Sex::Female);
    QCOMPARE(SosReference::sexFromGender(QStringLiteral("男")), Sex::Male);
    QCOMPARE(SosReference::sexFromGender(QString()), Sex::Unknown);
}

void MainWindowSafetyTests::patientResultUsesReferenceAndSkipsChildren()
{
    const QVector<double> rounds = {3910.048533906161, 3956.9673922545667,
        3946.3440860215032, 3951.612903225805, 4152.542372881357, 3983.7398373983733};
    const auto finish = [&rounds](MainWindow& window, const QTemporaryDir& directory,
                                  const PatientInfo& patient, double* finalSos) {
        PREPARE_ARCHIVE_WINDOW(window, directory);
        window.currentPatient = patient;
        window.patientList = {patient};
        window.measurementList.clear();
        window.session.roundSos = rounds;
        window.session.roundA = {10, 20, 30, 40, 5000, 60};
        window.session.roundB = {100, 200, 300, 400, 50000, 600};
        double a = 0, b = 0;
        QVERIFY(window.session.finalMeans(*finalSos, a, b));
        window.finishAllPatientRounds();
        QCOMPARE(window.measurementList.size(), 1);
    };

    {   // Adult woman: T against the female peak group, Z against her age.
        QTemporaryDir directory;
        QVERIFY(directory.isValid());
        MainWindow window;
        const PatientInfo patient = samplePatient();
        double sos = 0;
        finish(window, directory, patient, &sos);
        const int age = BoneHealth::calcPatientAge(QDate::fromString(patient.birthDay, "yyyy-MM-dd"));
        const MeasurementRecord& record = window.measurementList.last();
        const double t = SosReference::tScore(sos, SosReference::Sex::Female);
        QCOMPARE(record.tScore, QString::number(t, 'f', 2));
        QCOMPARE(record.zScore, QString::number(
            SosReference::zScore(sos, SosReference::Sex::Female, age), 'f', 2));
        QVERIFY(t < -1.0 && t > -2.5);
        QCOMPARE(record.boneStrength, QStringLiteral("不足"));
        QCOMPARE(record.diagnosis, QStringLiteral("不足"));
        QVERIFY(!record.fractureRisk.isEmpty());
        QVERIFY(!record.boneAge.isEmpty());
    }

    {   // Ten-year-old boy: SOS only, no adult-derived values.
        QTemporaryDir directory;
        QVERIFY(directory.isValid());
        MainWindow window;
        PatientInfo child = samplePatient();
        child.id = QStringLiteral("child-001");
        child.gender = QStringLiteral("男");
        child.birthDay = QDate::currentDate().addYears(-10).toString("yyyy-MM-dd");
        double sos = 0;
        finish(window, directory, child, &sos);
        const MeasurementRecord& record = window.measurementList.last();
        QCOMPARE(record.sos, QString::number(sos, 'f', 1));
        QVERIFY(record.tScore.isEmpty());
        QVERIFY(record.zScore.isEmpty());
        QVERIFY(record.fractureRisk.isEmpty());
        QVERIFY(record.boneAge.isEmpty());
        QCOMPARE(record.boneStrength, QStringLiteral("不评定"));
        QVERIFY(record.diagnosis.contains(QStringLiteral("未满20岁")));
        window.updateResultPanel();
        QCOMPARE(window.ui->lblLatestT->text(), QStringLiteral("--"));
        QCOMPARE(window.ui->lblLatestZ->text(), QStringLiteral("--"));
        QCOMPARE(window.ui->lblLatestBoneAge->text(), QStringLiteral("--"));
    }

    {   // Missing sex: no reference can be matched.
        QTemporaryDir directory;
        QVERIFY(directory.isValid());
        MainWindow window;
        PatientInfo unknown = samplePatient();
        unknown.gender.clear();
        double sos = 0;
        finish(window, directory, unknown, &sos);
        const MeasurementRecord& record = window.measurementList.last();
        QVERIFY(record.tScore.isEmpty());
        QVERIFY(record.diagnosis.contains(QStringLiteral("性别未填写")));
    }
}

void MainWindowSafetyTests::adultAgeSosChartIsDrawnFromReference()
{
    using Profile = AgeSosChartWidget::Profile;
    QCOMPARE(AgeSosChartWidget::profileFor(QStringLiteral("女"), 19), Profile::Girl);
    QCOMPARE(AgeSosChartWidget::profileFor(QStringLiteral("女"), 20), Profile::Woman);
    QCOMPARE(AgeSosChartWidget::profileFor(QStringLiteral("男"), 20), Profile::Man);

    // The visible SOS range is exactly T -5 … +3 of the same reference.
    for (const QString& gender : {QStringLiteral("女"), QStringLiteral("男")}) {
        const SosReference::Stat young = SosReference::youngAdult(SosReference::sexFromGender(gender));
        QVERIFY(AgeSosChartWidget::supportsPoint(gender, 50, young.mean - young.sd));
        QVERIFY(AgeSosChartWidget::supportsPoint(gender, 50, young.mean + 3.0 * young.sd));
        QVERIFY(AgeSosChartWidget::supportsPoint(gender, 50, young.mean - 5.0 * young.sd));
        QVERIFY(!AgeSosChartWidget::supportsPoint(gender, 50, young.mean + 3.05 * young.sd));
        QVERIFY(!AgeSosChartWidget::supportsPoint(gender, 50, young.mean - 5.05 * young.sd));
        QVERIFY(!AgeSosChartWidget::supportsPoint(gender, 101, young.mean));
    }

    const QString captureDir = qEnvironmentVariable("BONE_UI_CAPTURE_DIR");
    for (const QString& gender : {QStringLiteral("女"), QStringLiteral("男")}) {
        AgeSosChartData data;
        data.hasPatient = true;
        data.hasMeasurementRecords = true;
        data.gender = gender;
        data.focalAge = 66;
        data.points = {{58, 3990.0, QStringLiteral("2018-05-01T09:00:00"), false},
                       {62, 3935.0, QStringLiteral("2022-05-01T09:00:00"), false},
                       {66, 3870.0, QStringLiteral("2026-05-01T09:00:00"), true}};
        QImage image(607, 473, QImage::Format_ARGB32);
        image.fill(Qt::white);
        QPainter painter(&image);
        AgeSosChartWidget::renderChart(&painter, QRectF(image.rect()), data);
        painter.end();
        // Near T = +3 on the left: normal band; near T = -5: very-low band.
        QCOMPARE(image.pixelColor(60, 45), QColor(186, 255, 186));
        QCOMPARE(image.pixelColor(60, 425), QColor(255, 186, 186));
        if (!captureDir.isEmpty()) {
            QDir().mkpath(captureDir);
            QVERIFY(image.save(QDir(captureDir).filePath(
                gender == QStringLiteral("女") ? QStringLiteral("age-sos-woman.png")
                                              : QStringLiteral("age-sos-man.png"))));
        }
    }

    if (!captureDir.isEmpty()) {
        // Children keep the pediatric bitmap; capture one for visual review.
        AgeSosChartData child;
        child.hasPatient = true;
        child.hasMeasurementRecords = true;
        child.gender = QStringLiteral("女");
        child.focalAge = 10;
        child.points = {{10, 3700.0, QStringLiteral("2026-05-01T09:00:00"), true}};
        QImage image(607, 473, QImage::Format_ARGB32);
        image.fill(Qt::white);
        QPainter painter(&image);
        AgeSosChartWidget::renderChart(&painter, QRectF(image.rect()), child);
        painter.end();
        QVERIFY(image.save(QDir(captureDir).filePath(QStringLiteral("age-sos-girl.png"))));
    }
}

namespace {

// Centroid of the pixels whose colour is within `tolerance` (sum of channel
// differences) of `target`, inside `box`. Returns (-1, -1) if none match.
QPointF colourCentroid(const QImage& image, const QRect& box, const QColor& target, int tolerance)
{
    double sx = 0, sy = 0;
    int count = 0;
    const QRect area = box.intersected(image.rect());
    for (int y = area.top(); y <= area.bottom(); ++y) {
        for (int x = area.left(); x <= area.right(); ++x) {
            const QColor c = image.pixelColor(x, y);
            const int d = qAbs(c.red() - target.red()) + qAbs(c.green() - target.green())
                          + qAbs(c.blue() - target.blue());
            if (d <= tolerance) {
                sx += x;
                sy += y;
                ++count;
            }
        }
    }
    return count ? QPointF(sx / count, sy / count) : QPointF(-1, -1);
}

QImage renderAdultChartWithPoint(const QString& gender, int age, double sos)
{
    AgeSosChartData data;
    data.hasPatient = true;
    data.hasMeasurementRecords = true;
    data.gender = gender;
    data.focalAge = age;
    // A history point: no callout label to get in the way of the measurement.
    data.points = {{age, sos, QStringLiteral("2026-05-01T09:00:00"), false}};
    QImage image(607, 473, QImage::Format_ARGB32);
    image.fill(Qt::white);
    QPainter painter(&image);
    AgeSosChartWidget::renderChart(&painter, QRectF(image.rect()), data);
    painter.end();
    return image;
}

} // namespace

void MainWindowSafetyTests::adultChartPointsMatchComputedScores()
{
    using SosReference::Sex;
    const QColor historyPoint(140, 140, 140);
    const QColor thresholdLine(76, 222, 110);
    const QColor meanLine(40, 150, 220);
    const QRect belowLegend(0, 70, 607, 403);

    for (const QString& gender : {QStringLiteral("女"), QStringLiteral("男")}) {
        const Sex sex = SosReference::sexFromGender(gender);
        const SosReference::Stat young = SosReference::youngAdult(sex);

        // T = -1 exactly: the point must sit on the T = -1 line.
        const double tMinusOne = young.mean - young.sd;
        QVERIFY(qAbs(SosReference::tScore(tMinusOne, sex) + 1.0) < 1e-9);
        QImage image = renderAdultChartWithPoint(gender, 30, tMinusOne);
        const QPointF point = colourCentroid(image, belowLegend, historyPoint, 12);
        QVERIFY(point.x() > 0);
        const int lineColumn = qRound(point.x()) + 40;
        const QPointF line = colourCentroid(image, QRect(lineColumn, 70, 1, 403), thresholdLine, 50);
        QVERIFY(line.y() > 0);
        QVERIFY2(qAbs(point.y() - line.y()) <= 1.5,
                 qPrintable(QStringLiteral("%1: T=-1 point y=%2, T=-1 line y=%3")
                                .arg(gender).arg(point.y()).arg(line.y())));
    }

    // Z = 0 exactly on the steepest stretch of the female curve (47.5 → 52.5),
    // where placing the point half a year off would move it about 3 px.
    const int age = 49;
    const double zZero = SosReference::atAge(Sex::Female, age + 0.5).mean;
    QVERIFY(qAbs(SosReference::zScore(zZero, Sex::Female, age)) < 1e-9);
    const QImage image = renderAdultChartWithPoint(QStringLiteral("女"), age, zZero);
    const QPointF point = colourCentroid(image, belowLegend, historyPoint, 12);
    QVERIFY(point.x() > 0);
    // The curve is straight across this span, so the average of the curve just
    // left and right of the marker is the curve height under its centre.
    const int left = qRound(point.x()) - 12;
    const int right = qRound(point.x()) + 12;
    const QPointF meanLeft = colourCentroid(image, QRect(left, 70, 1, 403), meanLine, 40);
    const QPointF meanRight = colourCentroid(image, QRect(right, 70, 1, 403), meanLine, 40);
    QVERIFY(meanLeft.y() > 0 && meanRight.y() > 0);
    const double curveY = (meanLeft.y() + meanRight.y()) / 2.0;
    QVERIFY2(qAbs(point.y() - curveY) <= 1.5,
             qPrintable(QStringLiteral("Z=0 point y=%1, mean curve y=%2").arg(point.y()).arg(curveY)));
}

void MainWindowSafetyTests::savedRecordsAreShownWithCurrentReference()
{
    // The shared derivation used for new results and for saved records.
    const BoneHealth::DerivedResult adult = BoneHealth::deriveResult(4000.0, QStringLiteral("女"), 36);
    QCOMPARE(adult.tScore, QStringLiteral("-1.23"));   // (4000 - 4176.9) / 144.1
    QCOMPARE(adult.strength, QStringLiteral("不足"));
    QCOMPARE(adult.diagnosis, QStringLiteral("不足"));
    const BoneHealth::DerivedResult child = BoneHealth::deriveResult(3700.0, QStringLiteral("女"), 19);
    QVERIFY(child.tScore.isEmpty() && child.zScore.isEmpty() && child.boneAge.isEmpty());
    QCOMPARE(child.strength, QStringLiteral("不评定"));
    QVERIFY(BoneHealth::deriveResult(4000.0, QStringLiteral("女"), -1).diagnosis
                .contains(QStringLiteral("出生日期无效")));
    QVERIFY(BoneHealth::deriveResult(4000.0, QString(), 36).diagnosis
                .contains(QStringLiteral("性别未填写")));

    // A record saved with the old placeholder reference (T -0.85, 正常).
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    MainWindow window;
    PREPARE_ARCHIVE_WINDOW(window, directory);
    const PatientInfo patient = samplePatient();      // 女, born 1990-01-02
    MeasurementRecord saved = sampleMeasurement();     // 4000.0 m/s on 2026-07-22 → 36 years
    saved.tScore = QStringLiteral("-0.85");
    saved.zScore = QStringLiteral("-0.40");
    saved.boneStrength = QStringLiteral("正常");
    saved.diagnosis = QStringLiteral("正常");
    saved.fractureRisk = QStringLiteral("1.4");
    window.patientList = {patient};
    QVERIFY(window.savePatients(window.patientList));
    QVERIFY(window.selectCurrentPatient(patient));
    window.measurementList = {saved};
    window.updateResultPanel();

    QCOMPARE(window.ui->lblLatestT->text(), adult.tScore);
    QCOMPARE(window.ui->lblLatestZ->text(), adult.zScore);
    QCOMPARE(window.ui->lblLatestRisk->text(), adult.fractureRisk);
    QCOMPARE(window.ui->lblLatestStrength->text(), QStringLiteral("骨强度 不足"));

    const ReportData report = window.buildReportData(patient, saved);
    QCOMPARE(report.tScore, adult.tScore);
    QCOMPARE(report.zScore, adult.zScore);
    QCOMPARE(report.boneStrength, QStringLiteral("不足"));
    QVERIFY(report.diagnosis.contains(QStringLiteral("骨强度评估：不足")));
    QVERIFY(!report.diagnosis.contains(QStringLiteral("正常")));

    // Free-text diagnosis notes survive; only the generated verdict is replaced.
    MeasurementRecord noted = saved;
    noted.diagnosis = QStringLiteral("复查");
    QVERIFY(window.buildReportData(patient, noted).diagnosis.contains(QStringLiteral("复查")));

    // Nothing stored is rewritten.
    QCOMPARE(window.measurementList.first().tScore, QStringLiteral("-0.85"));
    QCOMPARE(window.measurementList.first().boneStrength, QStringLiteral("正常"));
}

void MainWindowSafetyTests::boneAgeAndChildChartStayConsistent()
{
    using SosReference::Sex;
    // Women: from the peak (47.5) on the curve falls strictly, so an SOS equal to
    // the curve at age + 0.5 reads back exactly that age — the bone age is the
    // point on the chart's mean curve at the same height as the result.
    for (int age = 47; age <= 77; ++age) {
        const double sos = SosReference::atAge(Sex::Female, age + 0.5).mean;
        QCOMPARE(BoneHealth::estimateBoneAgeFromSos(sos, QStringLiteral("女")), age);
    }
    // Above the peak: the peak age. Below the oldest group: the oldest age.
    QCOMPARE(BoneHealth::estimateBoneAgeFromSos(4400.0, QStringLiteral("女")), 47);
    QCOMPARE(BoneHealth::estimateBoneAgeFromSos(4400.0, QStringLiteral("男")), 42);
    QCOMPARE(BoneHealth::estimateBoneAgeFromSos(3600.0, QStringLiteral("女")), 77);
    QCOMPARE(BoneHealth::estimateBoneAgeFromSos(3600.0, QString()), -1);

    // A lower SOS never gives a younger bone age, for either sex.
    for (const QString& gender : {QStringLiteral("女"), QStringLiteral("男")}) {
        int previous = 0;
        for (double sos = 4400.0; sos >= 3500.0; sos -= 1.0) {
            const int boneAge = BoneHealth::estimateBoneAgeFromSos(sos, gender);
            QVERIFY2(boneAge >= previous,
                     qPrintable(QStringLiteral("%1 SOS %2: bone age %3 after %4")
                                    .arg(gender).arg(sos).arg(boneAge).arg(previous)));
            previous = boneAge;
        }
    }

    // Children are not scored, so their chart must not carry verdict colours.
    for (const QString& gender : {QStringLiteral("女"), QStringLiteral("男")}) {
        AgeSosChartData child;
        child.hasPatient = true;
        child.hasMeasurementRecords = true;
        child.gender = gender;
        child.focalAge = 3;
        child.points = {{3, 3300.0, QStringLiteral("2026-05-01T09:00:00"), true}};
        QImage image(607, 473, QImage::Format_ARGB32);
        image.fill(Qt::white);
        QPainter painter(&image);
        AgeSosChartWidget::renderChart(&painter, QRectF(image.rect()), child);
        painter.end();
        const QList<QColor> verdictColours = {QColor(186, 255, 186), QColor(255, 255, 186),
                                              QColor(255, 186, 186)};
        int verdictPixels = 0;
        for (int y = 0; y < image.height(); ++y)
            for (int x = 0; x < image.width(); ++x)
                if (verdictColours.contains(image.pixelColor(x, y))) ++verdictPixels;
        QCOMPARE(verdictPixels, 0);
        // The measured point is still drawn (below the legend, which also has a red dot).
        const QPointF point = colourCentroid(image, QRect(0, 70, 607, 403), QColor(245, 108, 108), 12);
        QVERIFY(point.x() > 0);
    }
}
