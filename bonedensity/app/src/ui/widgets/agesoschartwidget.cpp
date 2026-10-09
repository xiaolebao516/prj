#include "widgets/agesoschartwidget.h"

#include "health/sosreference.h"

#include <QFontMetricsF>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPolygonF>

#include <cmath>
#include <functional>

namespace {

// Proportions the age-SOS area has always been laid out at (the old reference
// bitmaps were 607 x 473), so the main-window row height stays the same.
constexpr double kChartAspect = 473.0 / 607.0;

// Pediatric SOS range shown on the children's chart.
constexpr double kChildMinSos = 2900.0;
constexpr double kChildMaxSos = 4300.0;

const QColor kNormalBand(186, 255, 186);
const QColor kLowBand(255, 255, 186);
const QColor kVeryLowBand(255, 186, 186);
const QColor kNormalLine(76, 222, 110);
const QColor kVeryLowLine(250, 96, 100);
const QColor kNeutralBackground(248, 250, 252);
const QColor kNeutralGrid(226, 231, 237);
const QColor kMeanLine(40, 150, 220);
const QColor kSdLine(104, 196, 240);
const QColor kSdFill(64, 170, 230, 60);
const QColor kAxisText(104, 110, 120);
const QColor kLatestPoint(245, 108, 108);
const QColor kHistoryPoint(126, 143, 160);

// Children's curves digitised from the research group's existing pediatric
// charts (formerly age_sos_girl.bmp / age_sos_boy.bmp; source not recorded):
// mean and half the ±1SD band, every half year. Display only — no score is
// computed for under-20s, so the chart has no T axis and no verdict bands.
struct ChildRow {
    double age;
    SosReference::Stat boy;
    SosReference::Stat girl;
};

const ChildRow kChildRows[] = {
    { 0.0, {3074.0, 104.0}, {3126.5, 106.6}},
    { 0.5, {3131.0, 104.0}, {3173.9, 106.6}},
    { 1.0, {3176.1, 105.0}, {3227.2, 106.6}},
    { 1.5, {3246.8, 104.0}, {3282.5, 106.6}},
    { 2.0, {3331.2, 105.0}, {3337.8, 106.6}},
    { 2.5, {3380.3, 104.0}, {3387.1, 107.6}},
    { 3.0, {3423.4, 104.0}, {3426.6, 106.6}},
    { 3.5, {3468.6, 104.0}, {3462.2, 106.6}},
    { 4.0, {3507.9, 104.0}, {3503.7, 106.6}},
    { 4.5, {3537.3, 106.0}, {3541.2, 106.6}},
    { 5.0, {3568.7, 106.0}, {3572.8, 106.6}},
    { 5.5, {3592.3, 104.0}, {3596.5, 107.6}},
    { 6.0, {3611.9, 104.0}, {3620.2, 106.6}},
    { 6.5, {3627.6, 106.0}, {3639.9, 106.6}},
    { 7.0, {3647.2, 104.0}, {3655.7, 106.6}},
    { 7.5, {3655.1, 104.0}, {3667.6, 105.7}},
    { 8.0, {3659.0, 104.0}, {3679.4, 105.7}},
    { 8.5, {3659.0, 105.0}, {3687.3, 106.6}},
    { 9.0, {3662.9, 104.0}, {3697.2, 106.6}},
    { 9.5, {3664.9, 105.0}, {3703.1, 107.6}},
    {10.0, {3664.9, 105.0}, {3711.0, 106.6}},
    {10.5, {3664.9, 105.0}, {3715.0, 107.6}},
    {11.0, {3659.0, 104.0}, {3720.9, 106.6}},
    {11.5, {3657.0, 104.0}, {3726.8, 106.6}},
    {12.0, {3651.1, 104.0}, {3738.7, 106.6}},
    {12.5, {3651.1, 104.0}, {3746.6, 107.6}},
    {13.0, {3651.1, 104.0}, {3758.4, 106.6}},
    {13.5, {3651.1, 104.0}, {3766.3, 106.6}},
    {14.0, {3651.1, 105.0}, {3782.1, 106.6}},
    {14.5, {3661.0, 104.0}, {3794.0, 107.6}},
    {15.0, {3666.9, 104.0}, {3809.8, 106.6}},
    {15.5, {3676.7, 104.0}, {3823.6, 106.6}},
    {16.0, {3688.4, 104.0}, {3843.3, 106.6}},
    {16.5, {3706.1, 105.0}, {3867.0, 106.6}},
    {17.0, {3725.7, 105.0}, {3890.7, 106.6}},
    {17.5, {3753.2, 104.0}, {3918.4, 106.6}},
    {18.0, {3772.9, 104.0}, {3951.9, 106.6}},
    {18.5, {3821.9, 105.0}, {3997.4, 108.6}},
    {19.0, {3867.1, 104.0}, {4044.8, 106.6}},
    {19.5, {3918.1, 104.0}, {4092.2, 106.6}},
    {20.0, {3957.4, 105.0}, {4139.6, 106.6}},
};
constexpr int kChildRowCount = int(sizeof(kChildRows) / sizeof(kChildRows[0]));

SosReference::Stat childAt(bool female, double age)
{
    const auto pick = [female](const ChildRow& row) { return female ? row.girl : row.boy; };
    if (age <= kChildRows[0].age) return pick(kChildRows[0]);
    for (int i = 0; i + 1 < kChildRowCount; ++i) {
        const ChildRow& lo = kChildRows[i];
        const ChildRow& hi = kChildRows[i + 1];
        if (age > hi.age) continue;
        const double k = (age - lo.age) / (hi.age - lo.age);
        const SosReference::Stat a = pick(lo);
        const SosReference::Stat b = pick(hi);
        return {a.mean + k * (b.mean - a.mean), a.sd + k * (b.sd - a.sd)};
    }
    return pick(kChildRows[kChildRowCount - 1]);
}

QFont chartFont(int pixelSize, bool bold = false)
{
    QFont font(QStringLiteral("Microsoft YaHei"));
    font.setPixelSize(pixelSize);
    font.setBold(bold);
    return font;
}

struct Frame {
    QRectF area;                                   // points and labels stay inside
    std::function<QPointF(double, double)> map;    // (age, sos) -> pixel
};

struct CurveChart {
    double minAge = 0.0;
    double maxAge = 0.0;
    double minSos = 0.0;
    double maxSos = 0.0;
    std::function<SosReference::Stat(double)> at;  // reference at an exact age
    double solidUntil = 0.0;      // past this age the curve is held flat (dashed)
    bool tAxis = false;           // adults: T axis, verdict bands, threshold lines
    SosReference::Stat young;     // T reference when tAxis
    QString caption;
};

void drawLegend(QPainter* painter, const QRectF& plot)
{
    const QFont font = chartFont(10);
    const QFontMetricsF fm(font);
    const QString latest = QStringLiteral("本次");
    const QString history = QStringLiteral("历史");
    const QString band = QStringLiteral("同龄均值 ±1SD");
    const qreal dot = 8, gap = 4, spacing = 10, swatch = 16;
    const qreal width = 12 + dot + gap + fm.horizontalAdvance(latest) + spacing
                        + dot + gap + fm.horizontalAdvance(history) + spacing
                        + swatch + gap + fm.horizontalAdvance(band) + 10;
    const qreal height = fm.height() + 8;
    if (width > plot.width() - 12) return;

    QRectF box(plot.right() - width - 6, plot.top() + 6, width, height);
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(255, 255, 255, 215));
    painter->drawRoundedRect(box, 4, 4);
    painter->setFont(font);

    qreal x = box.left() + 8;
    const qreal cy = box.center().y();
    const auto text = [&](const QString& value) {
        painter->setPen(kAxisText);
        const qreal w = fm.horizontalAdvance(value);
        painter->drawText(QRectF(x, box.top(), w + 1, box.height()), Qt::AlignVCenter, value);
        x += w + spacing;
    };
    painter->setPen(Qt::NoPen);
    painter->setBrush(kLatestPoint);
    painter->drawEllipse(QPointF(x + dot / 2, cy), dot / 2, dot / 2);
    x += dot + gap;
    text(latest);
    painter->setPen(Qt::NoPen);
    painter->setBrush(kHistoryPoint);
    painter->drawEllipse(QPointF(x + dot / 2, cy), dot / 2, dot / 2);
    x += dot + gap;
    text(history);
    painter->fillRect(QRectF(x, cy - 4, swatch, 8), kSdFill);
    painter->setPen(QPen(kMeanLine, 1.6));
    painter->drawLine(QPointF(x, cy), QPointF(x + swatch, cy));
    x += swatch + gap;
    text(band);
}

Frame drawReference(QPainter* painter, const QRectF& rect, const CurveChart& chart)
{
    const QFont tickFont = chartFont(11);
    const QFontMetricsF fm(tickFont);
    const QString unit = QStringLiteral("岁");
    const qreal left = fm.horizontalAdvance(chart.tAxis ? QStringLiteral("-5") : QStringLiteral("4300")) + 14;
    const qreal right = chart.tAxis ? fm.horizontalAdvance(QStringLiteral("4609")) + 14
                                    : fm.horizontalAdvance(unit) + 20;
    const qreal top = fm.height() + 10;
    // Age labels sit below the half-height of the lowest SOS label so the two
    // never touch in the bottom corners.
    const qreal ageRow = fm.height() / 2 + 3;
    const qreal bottom = ageRow + fm.height() + 2;
    const QRectF plot = rect.adjusted(left, top, -right, -bottom);

    const double minAge = chart.minAge, maxAge = chart.maxAge;
    const double minSos = chart.minSos, maxSos = chart.maxSos;
    const auto x = [plot, minAge, maxAge](double age) {
        return plot.left() + (age - minAge) / (maxAge - minAge) * plot.width();
    };
    const auto y = [plot, minSos, maxSos](double sos) {
        return plot.top() + (maxSos - sos) / (maxSos - minSos) * plot.height();
    };
    const SosReference::Stat young = chart.young;
    const auto yT = [&](double t) { return y(young.mean + t * young.sd); };

    painter->setFont(tickFont);
    if (chart.tAxis) {
        // T >= -1 normal, -2.5 < T < -1 low, T <= -2.5 very low.
        painter->fillRect(QRectF(plot.left(), plot.top(), plot.width(), yT(-1) - plot.top()), kNormalBand);
        painter->fillRect(QRectF(plot.left(), yT(-1), plot.width(), yT(-2.5) - yT(-1)), kLowBand);
        painter->fillRect(QRectF(plot.left(), yT(-2.5), plot.width(), plot.bottom() - yT(-2.5)), kVeryLowBand);
        for (int t = int(AgeSosChartWidget::minT); t <= int(AgeSosChartWidget::maxT); ++t) {
            const qreal yy = yT(t);
            if (t > AgeSosChartWidget::minT && t < AgeSosChartWidget::maxT) {
                painter->setPen(QPen(QColor(255, 255, 255, 190), 1));
                painter->drawLine(QPointF(plot.left(), yy), QPointF(plot.right(), yy));
            }
            painter->setPen(kAxisText);
            painter->drawText(QRectF(rect.left(), yy - fm.height() / 2, left - 6, fm.height()),
                              Qt::AlignRight | Qt::AlignVCenter, QString::number(t));
            painter->drawText(QRectF(plot.right() + 6, yy - fm.height() / 2, right - 6, fm.height()),
                              Qt::AlignLeft | Qt::AlignVCenter,
                              QString::number(qRound(young.mean + t * young.sd)));
        }
    } else {
        painter->fillRect(plot, kNeutralBackground);
        for (double sos = std::ceil(minSos / 200.0) * 200.0; sos <= maxSos; sos += 200.0) {
            const qreal yy = y(sos);
            painter->setPen(QPen(kNeutralGrid, 1));
            painter->drawLine(QPointF(plot.left(), yy), QPointF(plot.right(), yy));
            painter->setPen(kAxisText);
            painter->drawText(QRectF(rect.left(), yy - fm.height() / 2, left - 6, fm.height()),
                              Qt::AlignRight | Qt::AlignVCenter, QString::number(qRound(sos)));
        }
    }

    painter->setPen(kAxisText);
    const int ageStep = maxAge - minAge > 40 ? 10 : 2;
    for (int age = int(minAge); age <= int(maxAge); age += ageStep) {
        painter->drawText(QRectF(x(age) - 20, plot.bottom() + ageRow, 40, fm.height()),
                          Qt::AlignCenter, QString::number(age));
    }
    painter->drawText(QRectF(plot.right() + 14, plot.bottom() + ageRow, right, fm.height()),
                      Qt::AlignLeft | Qt::AlignVCenter, unit);

    const QRectF titleRow(rect.left(), rect.top(), rect.width(), top - 4);
    const QString sosTitle = QStringLiteral("SOS (m/s)");
    painter->drawText(titleRow, Qt::AlignLeft | Qt::AlignVCenter,
                      chart.tAxis ? QStringLiteral("T值") : sosTitle);
    if (chart.tAxis) painter->drawText(titleRow, Qt::AlignRight | Qt::AlignVCenter, sosTitle);
    const QFont captionFont = chartFont(10);
    painter->setFont(captionFont);
    const QFontMetricsF captionMetrics(captionFont);
    const qreal captionWidth = titleRow.width() - 2 * fm.horizontalAdvance(sosTitle) - 16;
    painter->drawText(titleRow, Qt::AlignCenter,
                      captionMetrics.elidedText(chart.caption, Qt::ElideRight, captionWidth));

    if (chart.tAxis) {
        painter->setPen(QPen(kNormalLine, 2.5));
        painter->drawLine(QPointF(plot.left(), yT(-1)), QPointF(plot.right(), yT(-1)));
        painter->setPen(QPen(kVeryLowLine, 2.5));
        painter->drawLine(QPointF(plot.left(), yT(-2.5)), QPointF(plot.right(), yT(-2.5)));
    }

    // Reference mean ±1 SD; dashed where the data is held flat.
    painter->save();
    painter->setClipRect(plot);
    const double solidUntil = qMin(maxAge, chart.solidUntil);
    const auto drawSegment = [&](double fromAge, double toAge, bool measured) {
        if (toAge <= fromAge) return;
        QPolygonF upper, mean, lower;
        for (double age = fromAge;; age += 0.25) {
            const double a = qMin(age, toAge);
            const SosReference::Stat ref = chart.at(a);
            upper << QPointF(x(a), y(ref.mean + ref.sd));
            mean << QPointF(x(a), y(ref.mean));
            lower << QPointF(x(a), y(ref.mean - ref.sd));
            if (a >= toAge) break;
        }
        QPainterPath band;
        band.addPolygon(upper);
        for (int i = lower.size() - 1; i >= 0; --i) band.lineTo(lower.at(i));
        band.closeSubpath();
        QColor fill = kSdFill;
        if (!measured) fill.setAlpha(fill.alpha() / 2);
        painter->fillPath(band, fill);
        const Qt::PenStyle style = measured ? Qt::SolidLine : Qt::DashLine;
        painter->setBrush(Qt::NoBrush);
        painter->setPen(QPen(kSdLine, 1.3, style));
        painter->drawPolyline(upper);
        painter->drawPolyline(lower);
        painter->setPen(QPen(kMeanLine, 2.0, style));
        painter->drawPolyline(mean);
    };
    drawSegment(minAge, solidUntil, true);
    drawSegment(solidUntil, maxAge, false);
    painter->restore();

    painter->setPen(QPen(QColor(196, 202, 210), 1));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(plot);
    drawLegend(painter, plot);

    // Records carry whole years; Z compares them with the reference at age + 0.5
    // (SosReference::zScore), so the point is placed there too. A Z = 0 result then
    // sits on the mean curve and a T = -1 result on the T = -1 line.
    return {plot, [x, y, maxAge](double age, double sos) {
        return QPointF(x(qMin(age + 0.5, maxAge)), y(sos));
    }};
}

CurveChart adultChart(SosReference::Sex sex)
{
    CurveChart chart;
    chart.young = SosReference::youngAdult(sex);
    chart.minAge = SosReference::kAdultMinAge;
    chart.maxAge = 100.0;
    chart.minSos = chart.young.mean + AgeSosChartWidget::minT * chart.young.sd;
    chart.maxSos = chart.young.mean + AgeSosChartWidget::maxT * chart.young.sd;
    chart.at = [sex](double age) { return SosReference::atAge(sex, age); };
    // The table's last group is "75 and older", so past its midpoint the curve is flat.
    chart.solidUntil = SosReference::lastGroupMidAge();
    chart.tAxis = true;
    chart.caption = SosReference::sourceLabel();
    return chart;
}

CurveChart childChart(bool female)
{
    CurveChart chart;
    chart.minAge = 0.0;
    chart.maxAge = SosReference::kAdultMinAge;
    chart.minSos = kChildMinSos;
    chart.maxSos = kChildMaxSos;
    chart.at = [female](double age) { return childAt(female, age); };
    chart.solidUntil = chart.maxAge;
    chart.caption = QStringLiteral("儿童参考曲线 · 未满20岁不评定");
    return chart;
}

} // namespace

AgeSosChartWidget::AgeSosChartWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(240, 180);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void AgeSosChartWidget::clearReferenceData()
{
    setChartData(AgeSosChartData());
}

void AgeSosChartWidget::setReferenceData(const QString& gender,
                                         int age,
                                         bool hasMeasurement,
                                         const QString& sosText)
{
    AgeSosChartData data;
    data.hasPatient = true;
    data.hasMeasurementRecords = hasMeasurement;
    data.gender = gender;
    data.focalAge = age;
    bool ok = false;
    const double parsedSos = sosText.trimmed().toDouble(&ok);
    if (hasMeasurement && ok && supportsPoint(gender, age, parsedSos)) {
        data.points.append({age, parsedSos, QString(), true});
    }
    setChartData(data);
}

void AgeSosChartWidget::setChartData(const AgeSosChartData& data)
{
    data_ = data;
    profile_ = profileFor(data.gender, data.focalAge);
    update();
}

const AgeSosChartData& AgeSosChartWidget::chartData() const
{
    return data_;
}

AgeSosChartWidget::Profile AgeSosChartWidget::profile() const
{
    return profile_;
}

double AgeSosChartWidget::imageAspectRatio() const
{
    return kChartAspect;
}

AgeSosChartWidget::Profile AgeSosChartWidget::profileFor(const QString& gender, int age)
{
    if (age < 0 || age > 100) return Profile::None;

    const bool female = gender.contains(QStringLiteral("女"));
    const bool male = gender.contains(QStringLiteral("男"));
    if (!female && !male) return Profile::None;

    if (age < SosReference::kAdultMinAge) return female ? Profile::Girl : Profile::Boy;
    return female ? Profile::Woman : Profile::Man;
}

bool AgeSosChartWidget::supportsPoint(const QString& gender, int age, double sos)
{
    if (!std::isfinite(sos)) return false;
    const ChartSpec spec = specFor(profileFor(gender, age));
    return spec.profile != Profile::None &&
           age >= spec.minAge && age <= spec.maxAge &&
           sos >= spec.minSos && sos <= spec.maxSos;
}

AgeSosChartWidget::ChartSpec AgeSosChartWidget::specFor(Profile profile)
{
    ChartSpec spec;
    spec.profile = profile;
    switch (profile) {
    case Profile::Girl:
    case Profile::Boy:
        spec.minAge = 0.0;
        spec.maxAge = SosReference::kAdultMinAge;
        spec.minSos = kChildMinSos;
        spec.maxSos = kChildMaxSos;
        return spec;
    case Profile::Woman:
    case Profile::Man: {
        const SosReference::Stat young = SosReference::youngAdult(
            profile == Profile::Woman ? SosReference::Sex::Female : SosReference::Sex::Male);
        spec.minAge = SosReference::kAdultMinAge;
        spec.maxAge = 100.0;
        spec.minSos = young.mean + minT * young.sd;
        spec.maxSos = young.mean + maxT * young.sd;
        return spec;
    }
    case Profile::None:
        break;
    }
    return {};
}

void AgeSosChartWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    renderChart(&painter, rect(), data_);
}

void AgeSosChartWidget::renderChart(QPainter* painter,
                                    const QRectF& targetRect,
                                    const AgeSosChartData& data)
{
    if (!painter || targetRect.isEmpty()) return;
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->fillRect(targetRect, Qt::white);

    const auto drawCentered = [painter, &targetRect](const QString& text) {
        painter->setPen(QColor(144, 147, 153));
        painter->setFont(chartFont(14));
        painter->drawText(targetRect, Qt::AlignCenter, text);
    };

    if (!data.hasPatient) {
        drawCentered(QStringLiteral("选择被测者后显示对应参考曲线"));
        painter->restore();
        return;
    }

    const Profile profile = profileFor(data.gender, data.focalAge);
    if (profile == Profile::None) {
        drawCentered(QStringLiteral("被测者性别或出生日期不完整"));
        painter->restore();
        return;
    }

    const QRectF available = targetRect.adjusted(imageMargin, imageMargin, -imageMargin, -imageMargin);
    const CurveChart chart =
        profile == Profile::Woman ? adultChart(SosReference::Sex::Female)
        : profile == Profile::Man ? adultChart(SosReference::Sex::Male)
                                  : childChart(profile == Profile::Girl);
    const Frame frame = drawReference(painter, available, chart);

    bool drewPoint = false;
    AgeSosMeasurementPoint highlighted;
    bool hasHighlighted = false;
    for (int pass = 0; pass < 2; ++pass) {
        for (const AgeSosMeasurementPoint& value : data.points) {
            if (value.highlighted != (pass == 1) ||
                profileFor(data.gender, value.age) != profile ||
                !supportsPoint(data.gender, value.age, value.sos)) continue;
            drewPoint = true;
            const QPointF point = frame.map(value.age, value.sos);
            painter->setPen(Qt::NoPen);
            painter->setBrush(Qt::white);
            painter->drawEllipse(point, value.highlighted ? 8.5 : 7.0,
                                  value.highlighted ? 8.5 : 7.0);
            painter->setBrush(value.highlighted ? kLatestPoint : kHistoryPoint);
            painter->drawEllipse(point, value.highlighted ? 6.0 : 4.8,
                                  value.highlighted ? 6.0 : 4.8);
            if (value.highlighted) {
                highlighted = value;
                hasHighlighted = true;
            }
        }
    }

    if (hasHighlighted) {
        const QPointF point = frame.map(highlighted.age, highlighted.sos);
        const QString label = QStringLiteral("本次：%1岁  %2 m/s")
                                  .arg(highlighted.age)
                                  .arg(highlighted.sos, 0, 'f', 1);
        const QFont labelFont = chartFont(12);
        painter->setFont(labelFont);
        const QFontMetricsF metrics(labelFont);
        const QSizeF labelSize(metrics.horizontalAdvance(label) + 16, metrics.height() + 8);
        qreal labelX = point.x() + 10;
        if (labelX + labelSize.width() > frame.area.right() - 4)
            labelX = point.x() - labelSize.width() - 10;
        qreal labelY = point.y() - labelSize.height() - 8;
        if (labelY < frame.area.top() + 4)
            labelY = point.y() + 10;
        const QRectF labelRect(QPointF(labelX, labelY), labelSize);
        painter->setPen(kLatestPoint);
        painter->setBrush(QColor(255, 255, 255, 235));
        painter->drawRoundedRect(labelRect, 4, 4);
        painter->drawText(labelRect, Qt::AlignCenter, label);
    }

    if (!drewPoint) {
        painter->setFont(chartFont(12));
        const QString text = data.hasMeasurementRecords
            ? QStringLiteral("当前年龄分组暂无有效结果")
            : QStringLiteral("暂无检测结果");
        const qreal width = qMin<qreal>(220, frame.area.width() - 16);
        const QRectF statusRect(frame.area.center().x() - width / 2, frame.area.bottom() - 40,
                                width, 28);
        painter->setPen(QColor(144, 147, 153));
        painter->setBrush(QColor(255, 255, 255, 230));
        painter->drawRoundedRect(statusRect, 4, 4);
        painter->drawText(statusRect, Qt::AlignCenter, text);
    }

    if (data.omittedOtherProfileCount > 0) {
        painter->setFont(chartFont(11));
        painter->setPen(QColor(96, 98, 102));
        painter->drawText(QRectF(frame.area.left() + 8, frame.area.bottom() - 24,
                                 frame.area.width() - 16, 20),
                          Qt::AlignLeft | Qt::AlignVCenter,
                          QStringLiteral("另有 %1 条其他年龄阶段记录未在本图显示")
                              .arg(data.omittedOtherProfileCount));
    }
    painter->restore();
}
