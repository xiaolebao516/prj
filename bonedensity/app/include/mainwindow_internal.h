#pragma once

// Shared by the mainwindow_*.cpp files only (one MainWindow class split by area).

#include "agesoschartwidget.h"
#include "bonehealth.h"
#include "datalocation.h"
#include "parametergroup.h"
#include "types.h"

#include <QDate>
#include <QDateTime>
#include <QFontMetrics>
#include <QLabel>
#include <QResizeEvent>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace mainwindow_detail {

// ======================================================
// 逐帧调试开关：改为 true 可恢复每帧详细 qDebug 输出
// （波形数据、首波定位、角度特征、稳定性状态等）
// 正常使用时保持 false，只看每轮结束的闸门统计汇总
// ======================================================
inline constexpr bool kDebugPerFrame = false;


// A label that gives up its width first: the text is shortened with "…" and
// the full text stays in the tooltip, so neighbouring buttons keep their labels.
class ElidingLabel : public QLabel
{
public:
    using QLabel::QLabel;
    void setFullText(const QString& text)
    {
        fullText_ = text;
        setToolTip(text);
        updateGeometry();
        refresh();
    }
    const QString& fullText() const { return fullText_; }
    QSize minimumSizeHint() const override
    {
        return {fontMetrics().horizontalAdvance(QStringLiteral("测试…")), QLabel::minimumSizeHint().height()};
    }
    QSize sizeHint() const override
    {
        return {fontMetrics().horizontalAdvance(fullText_) + 4, QLabel::sizeHint().height()};
    }

protected:
    void resizeEvent(QResizeEvent* event) override
    {
        QLabel::resizeEvent(event);
        refresh();
    }

private:
    void refresh()
    {
        QLabel::setText(fontMetrics().elidedText(fullText_, Qt::ElideRight, qMax(0, width())));
    }
    QString fullText_;
};

inline int ageOnDate(const QString& birthDay, const QDate& date)
{
    const QDate birth = QDate::fromString(birthDay, QStringLiteral("yyyy-MM-dd"));
    if (!birth.isValid() || !date.isValid() || date < birth) return -1;

    int age = date.year() - birth.year();
    if (birth.addYears(age) > date) --age;
    return age;
}

inline QDateTime parsedMeasurementDateTime(const QString& value)
{
    QDateTime dateTime = QDateTime::fromString(value, Qt::ISODate);
    if (!dateTime.isValid())
        dateTime = QDateTime::fromString(value, QStringLiteral("yyyy-MM-dd HH:mm"));
    return dateTime;
}

inline int measurementAge(const MeasurementRecord& record, const PatientInfo& patient)
{
    bool ok = false;
    const int storedAge = record.patientAge.trimmed().toInt(&ok);
    if (ok && storedAge >= 0 && storedAge <= 100) return storedAge;

    const QDateTime measuredAt = parsedMeasurementDateTime(record.measuredAt);
    const QString birthDay = record.patientBirthDay.trimmed().isEmpty()
        ? patient.birthDay : record.patientBirthDay;
    return ageOnDate(birthDay, measuredAt.date());
}

inline QString measurementGender(const MeasurementRecord& record, const PatientInfo& patient)
{
    return record.patientGender.trimmed().isEmpty()
        ? patient.gender : record.patientGender;
}

// Saved records keep the values computed when they were measured; screens and
// reports show them re-derived from the stored SOS with the current reference,
// so the numbers always agree with the age-SOS chart.
inline MeasurementRecord withCurrentReference(const MeasurementRecord& record, const PatientInfo& patient)
{
    bool ok = false;
    const double sos = record.sos.trimmed().toDouble(&ok);
    if (!ok || !std::isfinite(sos) || sos <= 0.0) return record;

    const BoneHealth::DerivedResult derived = BoneHealth::deriveResult(
        sos, measurementGender(record, patient), measurementAge(record, patient));
    MeasurementRecord shown = record;
    // A diagnosis that is just the generated verdict is replaced; free text is kept.
    if (record.diagnosis.trimmed().isEmpty() || record.diagnosis == record.boneStrength)
        shown.diagnosis = derived.diagnosis;
    shown.tScore = derived.tScore;
    shown.zScore = derived.zScore;
    shown.boneStrength = derived.strength;
    shown.fractureRisk = derived.fractureRisk;
    shown.boneAge = derived.boneAge;
    return shown;
}

inline bool sameMeasurement(const MeasurementRecord& left, const MeasurementRecord& right)
{
    if (!left.id.trimmed().isEmpty() && !right.id.trimmed().isEmpty())
        return left.id == right.id;
    return left.patientId == right.patientId &&
           left.measuredAt == right.measuredAt &&
           left.sos == right.sos;
}

inline bool sameSexProfile(AgeSosChartWidget::Profile left, AgeSosChartWidget::Profile right)
{
    const bool leftFemale = left == AgeSosChartWidget::Profile::Girl ||
                            left == AgeSosChartWidget::Profile::Woman;
    const bool rightFemale = right == AgeSosChartWidget::Profile::Girl ||
                             right == AgeSosChartWidget::Profile::Woman;
    const bool leftMale = left == AgeSosChartWidget::Profile::Boy ||
                          left == AgeSosChartWidget::Profile::Man;
    const bool rightMale = right == AgeSosChartWidget::Profile::Boy ||
                           right == AgeSosChartWidget::Profile::Man;
    return (leftFemale && rightFemale) || (leftMale && rightMale);
}

} // namespace mainwindow_detail
