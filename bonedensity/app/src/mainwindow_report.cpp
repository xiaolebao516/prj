// 检测报表页面、报表数据与打印

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "bonehealth.h"
#include "sosreference.h"
#include "agesoschartwidget.h"
#include "reportwidget.h"
#include "calibrationdialog.h"
#include "measurementguidedialog.h"
#include "utils.h"
#include "patientformdialog.h"
#include "databackup.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDoubleValidator>
#include <QResizeEvent>
#include <QCheckBox>
#include <QDateEdit>
#include <QHeaderView>
#include <QScreen>
#include <QSignalBlocker>
#include <QStyle>
#include <QtCharts/QValueAxis>
#include <QRegularExpression>
#include <QInputDialog>
#include <QDateTime>
#include <QCoreApplication>
#include <QDomElement>
#include <QFileDialog>
#include <QFileInfo>
#include <algorithm>
#include <utility>
#include <cmath> // 确保包含 math 头文件
#include <limits>
#include <QFrame>
#include <QGridLayout>
#include <QBoxLayout>
#include <QPointer>
#include <QDialog>
#include <QDialogButtonBox>
#include <QTableWidget>
#include <QPushButton>
#include <QToolButton>
#include <QSettings>
#include <QSaveFile>
#include <QMenu>
#include <QDesktopServices>
#include <QUrl>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QUuid>
#include <QAction>
#include <QMenuBar>
#include <QSet>
#include <QScopeGuard>
#include <QJsonArray>
#include <QStandardPaths>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrinter>
#include "mainwindow_internal.h"

using namespace mainwindow_detail;


void MainWindow::setupReportPage()
{
    QVBoxLayout* pageLayout = new QVBoxLayout(ui->pageReport);
    pageLayout->setContentsMargins(16, 12, 16, 12);
    pageLayout->setSpacing(10);

    QHBoxLayout* toolbar = new QHBoxLayout();
    QPushButton* backButton = new QPushButton("返回", ui->pageReport);
    backButton->setObjectName(QStringLiteral("reportBackButton"));
    QPushButton* exportButton = new QPushButton("导出 PDF", ui->pageReport);
    QPushButton* printButton = new QPushButton("打印", ui->pageReport);
    toolbar->addWidget(backButton);
    toolbar->addStretch();
    toolbar->addWidget(exportButton);
    toolbar->addWidget(printButton);
    pageLayout->addLayout(toolbar);

    reportWidget = new ReportWidget(ui->pageReport);
    pageLayout->addWidget(reportWidget, 1);

    connect(backButton, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(reportReturnPage ? reportReturnPage : ui->pageMain);
        scheduleResponsiveLayout();
    });
    connect(printButton, &QPushButton::clicked, this, [this]() {
        QPrinter printer(QPrinter::HighResolution);
        printer.setPageSize(QPageSize(QPageSize::A4));
        printer.setPageOrientation(QPageLayout::Portrait);
        printer.setPageMargins(QMarginsF(8, 8, 8, 8), QPageLayout::Millimeter);
        QPrintDialog dialog(&printer, this);
        dialog.setWindowTitle("打印检测报表");
        if (dialog.exec() == QDialog::Accepted && !renderReportToPrinter(&printer)) {
            QMessageBox::warning(this, "打印失败", "无法生成打印内容，请检查打印机设置后重试。");
        }
    });
    connect(exportButton, &QPushButton::clicked, this, [this]() {
        const ReportData& data = reportWidget->reportData();
        QString fileName = QString("%1_%2_%3.pdf")
                               .arg(data.patientName, data.patientId,
                                    data.measuredAt.left(10));
        fileName.replace(QRegularExpression(R"([\\/:*?\"<>|])"), "_");
        const QString defaultPath = QStandardPaths::writableLocation(
                                        QStandardPaths::DocumentsLocation)
                                    + "/" + fileName;
        QString outputPath = QFileDialog::getSaveFileName(
            this, "导出检测报表", defaultPath, "PDF 文件 (*.pdf)");
        if (outputPath.isEmpty()) return;
        if (!outputPath.endsWith(".pdf", Qt::CaseInsensitive)) outputPath += ".pdf";

        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(outputPath);
        printer.setPageSize(QPageSize(QPageSize::A4));
        printer.setPageOrientation(QPageLayout::Portrait);
        printer.setPageMargins(QMarginsF(8, 8, 8, 8), QPageLayout::Millimeter);
        if (!renderReportToPrinter(&printer)) {
            QMessageBox::warning(this, "导出失败", "无法写入 PDF 文件，请检查保存位置后重试。");
            return;
        }
        QMessageBox::information(this, "导出完成", "检测报表已保存为 PDF 文件。");
    });
}

ReportData MainWindow::buildReportData(const PatientInfo& patient,
                                       const MeasurementRecord& savedMeasurement) const
{
    const MeasurementRecord measurement = withCurrentReference(savedMeasurement, patient);
    auto fallback = [](const QString& snapshot, const QString& current) {
        return snapshot.trimmed().isEmpty() ? current : snapshot;
    };
    auto withUnit = [](const QString& value, const QString& unit) {
        return value.trimmed().isEmpty() ? QString() : value + " " + unit;
    };

    ReportData data;
    data.patientName = fallback(measurement.patientName, patient.name);
    data.patientId = measurement.patientId.isEmpty() ? patient.id : measurement.patientId;
    data.gender = fallback(measurement.patientGender, patient.gender);
    data.birthDay = fallback(measurement.patientBirthDay, patient.birthDay);
    data.height = withUnit(fallback(measurement.patientHeight, patient.height), "cm");
    data.weight = withUnit(fallback(measurement.patientWeight, patient.weight), "kg");
    data.part = measurement.part;
    data.sos = withUnit(measurement.sos, "m/s");
    data.tScore = measurement.tScore;
    data.zScore = measurement.zScore;
    data.boneStrength = measurement.boneStrength.isEmpty()
        ? measurement.diagnosis : measurement.boneStrength;
    data.operatorName = measurement.operatorName;

    const QDateTime measuredDateTime = QDateTime::fromString(measurement.measuredAt, Qt::ISODate);
    data.measuredAt = measuredDateTime.isValid()
        ? measuredDateTime.toString("yyyy-MM-dd HH:mm") : measurement.measuredAt;

    data.age = measurement.patientAge;
    if (data.age.isEmpty() && measuredDateTime.isValid()) {
        const QDate birthDate = QDate::fromString(data.birthDay, "yyyy-MM-dd");
        const int age = ageOnDate(data.birthDay, measuredDateTime.date());
        if (birthDate.isValid() && age >= 0) {
            data.age = QString::number(age);
        }
    }

    QStringList diagnosisParts;
    if (!data.boneStrength.isEmpty()) {
        diagnosisParts << "骨强度评估：" + data.boneStrength;
    }
    diagnosisParts << "相对骨折风险："
                          + (measurement.fractureRisk.trimmed().isEmpty()
                                 ? QStringLiteral("--")
                                 : measurement.fractureRisk);
    diagnosisParts << (measurement.boneAge.isEmpty()
                           ? QStringLiteral("相对骨龄：--")
                           : QStringLiteral("相对骨龄：%1 岁").arg(measurement.boneAge));
    if (!measurement.diagnosis.isEmpty() && measurement.diagnosis != data.boneStrength) {
        diagnosisParts << measurement.diagnosis;
    }
    data.diagnosis = diagnosisParts.join("；");
    data.ageSosChart = buildAgeSosChartData(patient, measurement, true);
    return data;
}

AgeSosChartData MainWindow::buildAgeSosChartData(
    const PatientInfo& patient,
    const MeasurementRecord& focalMeasurement,
    bool cutoffAtFocal) const
{
    AgeSosChartData data;
    data.hasPatient = !patient.id.trimmed().isEmpty();
    data.gender = measurementGender(focalMeasurement, patient);
    data.focalAge = measurementAge(focalMeasurement, patient);
    const AgeSosChartWidget::Profile focalProfile =
        AgeSosChartWidget::profileFor(data.gender, data.focalAge);
    const QDateTime focalDateTime = parsedMeasurementDateTime(focalMeasurement.measuredAt);

    QList<MeasurementRecord> candidates = measurementsForPatient(patient.id);
    bool focalPresent = false;
    for (const MeasurementRecord& record : std::as_const(candidates)) {
        if (sameMeasurement(record, focalMeasurement)) {
            focalPresent = true;
            break;
        }
    }
    if (!focalPresent) candidates.append(focalMeasurement);

    for (const MeasurementRecord& record : std::as_const(candidates)) {
        const bool isFocal = sameMeasurement(record, focalMeasurement);
        const QDateTime recordDateTime = parsedMeasurementDateTime(record.measuredAt);
        if (cutoffAtFocal) {
            if (focalDateTime.isValid()) {
                if (!isFocal && (!recordDateTime.isValid() || recordDateTime > focalDateTime))
                    continue;
            } else if (!isFocal) {
                continue;
            }
        }

        data.hasMeasurementRecords = true;
        const int age = measurementAge(record, patient);
        const QString gender = measurementGender(record, patient);
        const AgeSosChartWidget::Profile recordProfile =
            AgeSosChartWidget::profileFor(gender, age);
        bool sosOk = false;
        const double sos = record.sos.trimmed().toDouble(&sosOk);
        if (!sosOk || !std::isfinite(sos) || recordProfile == AgeSosChartWidget::Profile::None)
            continue;

        if (recordProfile != focalProfile) {
            if (sameSexProfile(recordProfile, focalProfile) &&
                AgeSosChartWidget::supportsPoint(gender, age, sos)) {
                ++data.omittedOtherProfileCount;
            }
            continue;
        }
        if (!AgeSosChartWidget::supportsPoint(gender, age, sos)) continue;
        data.points.append({age, sos, record.measuredAt, isFocal});
    }

    std::stable_sort(data.points.begin(), data.points.end(),
                     [](const AgeSosMeasurementPoint& left,
                        const AgeSosMeasurementPoint& right) {
        const QDateTime leftTime = parsedMeasurementDateTime(left.measuredAt);
        const QDateTime rightTime = parsedMeasurementDateTime(right.measuredAt);
        if (leftTime.isValid() && rightTime.isValid()) return leftTime < rightTime;
        if (leftTime.isValid() != rightTime.isValid()) return leftTime.isValid();
        return left.measuredAt < right.measuredAt;
    });
    return data;
}

void MainWindow::showReport(const PatientInfo& patient,
                            const MeasurementRecord& measurement)
{
    reportWidget->setReportData(buildReportData(patient, measurement));
    ui->stackedWidget->setCurrentWidget(ui->pageReport);
}

bool MainWindow::renderReportToPrinter(QPrinter* printer)
{
    if (!printer || !reportWidget) return false;
    QPainter painter;
    if (!painter.begin(printer)) return false;
    const QRect pageRect = printer->pageLayout().paintRectPixels(printer->resolution());
    reportWidget->renderReport(&painter, pageRect);
    return painter.end();
}

void MainWindow::showReportFrom(QWidget* returnPage, const PatientInfo& patient,
                                const MeasurementRecord& measurement)
{
    reportReturnPage = returnPage;
    showReport(patient, measurement);
}
