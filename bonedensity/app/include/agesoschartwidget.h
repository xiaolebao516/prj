#pragma once

#include <QPixmap>
#include <QPointF>
#include <QRectF>
#include <QString>
#include <QWidget>

class QPainter;

class AgeSosChartWidget : public QWidget
{
public:
    enum class Profile {
        None,
        Girl,
        Boy,
        Woman,
        Man
    };

    explicit AgeSosChartWidget(QWidget* parent = nullptr);

    void clearReferenceData();
    void setReferenceData(const QString& gender,
                          int age,
                          bool hasMeasurement,
                          const QString& sosText = QString());
    Profile profile() const;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    struct ChartSpec {
        Profile profile = Profile::None;
        QString resourcePath;
        double minAge = 0.0;
        double maxAge = 0.0;
        double minSos = 0.0;
        double maxSos = 0.0;
        double xAtMinAge = 0.0;
        double xAtMaxAge = 0.0;
        double yAtMaxSos = 0.0;
        double yAtMinSos = 0.0;
    };

    static Profile profileFor(const QString& gender, int age);
    static ChartSpec specFor(Profile profile);
    QRectF fittedImageRect() const;
    bool measurementPoint(QPointF* point) const;
    void drawStatus(QPainter* painter, const QString& text) const;

    bool hasPatient_ = false;
    bool hasMeasurement_ = false;
    bool validSos_ = false;
    int age_ = -1;
    double sos_ = 0.0;
    Profile profile_ = Profile::None;
    ChartSpec spec_;
    QPixmap background_;
};
