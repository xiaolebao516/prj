#pragma once

#include <QPixmap>
#include <QString>
#include <QWidget>

class QPainter;

struct ReportData {
    QString patientName;
    QString patientId;
    QString age;
    QString gender;
    QString birthDay;
    QString measuredAt;
    QString height;
    QString weight;
    QString part;
    QString sos;
    QString tScore;
    QString zScore;
    QString boneStrength;
    QString diagnosis;
    QString operatorName;
};

class ReportWidget : public QWidget {
    Q_OBJECT
public:
    explicit ReportWidget(QWidget* parent = nullptr);

    void setReportData(const ReportData& data);
    const ReportData& reportData() const;
    void renderReport(QPainter* painter, const QRectF& targetRect) const;
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void drawLogicalPage(QPainter* painter) const;

    ReportData data_;
    QPixmap template_;
};
