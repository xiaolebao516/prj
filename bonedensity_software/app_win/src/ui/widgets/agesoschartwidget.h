#pragma once

#include <QPointF>
#include <QRectF>
#include <QString>
#include <QVector>
#include <QWidget>

class QPainter;

struct AgeSosMeasurementPoint {
    int age = -1;
    double sos = 0.0;
    QString measuredAt;
    bool highlighted = false;
};

struct AgeSosChartData {
    bool hasPatient = false;
    bool hasMeasurementRecords = false;
    QString gender;
    int focalAge = -1;
    QVector<AgeSosMeasurementPoint> points;
    int omittedOtherProfileCount = 0;
};

// Adults (20+) are drawn from SosReference, so the T axis, bands and mean curve
// match the reported T/Z, strength and bone age. Children get the group's
// pediatric curves without T axis or verdict bands, since under-20s are not scored.
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

    // Report keeps the printed report's colours; Screen is the softer on-screen
    // look (legend lives in the card header, verdict bands are labelled).
    enum class RenderStyle {
        Report,
        Screen
    };

    explicit AgeSosChartWidget(QWidget* parent = nullptr);

    void clearReferenceData();
    void setReferenceData(const QString& gender,
                          int age,
                          bool hasMeasurement,
                          const QString& sosText = QString());
    void setChartData(const AgeSosChartData& data);
    const AgeSosChartData& chartData() const;
    Profile profile() const;
    // Height/width the chart is laid out at (layout hint only).
    double imageAspectRatio() const;
    static constexpr int imageMargin = 6;
    // T range shown on the adult chart.
    static constexpr double minT = -5.0;
    static constexpr double maxT = 3.0;
    static Profile profileFor(const QString& gender, int age);
    static bool supportsPoint(const QString& gender, int age, double sos);
    static void renderChart(QPainter* painter,
                            const QRectF& targetRect,
                            const AgeSosChartData& data,
                            RenderStyle style = RenderStyle::Report);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    struct ChartSpec {
        Profile profile = Profile::None;
        double minAge = 0.0;
        double maxAge = 0.0;
        double minSos = 0.0;
        double maxSos = 0.0;
    };

    static ChartSpec specFor(Profile profile);

    Profile profile_ = Profile::None;
    AgeSosChartData data_;
};
