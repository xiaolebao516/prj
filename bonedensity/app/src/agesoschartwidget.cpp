#include "agesoschartwidget.h"

#include <QPaintEvent>
#include <QPainter>
#include <QFontMetrics>

#include <cmath>

AgeSosChartWidget::AgeSosChartWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(240, 180);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void AgeSosChartWidget::clearReferenceData()
{
    hasPatient_ = false;
    hasMeasurement_ = false;
    validSos_ = false;
    age_ = -1;
    sos_ = 0.0;
    profile_ = Profile::None;
    spec_ = ChartSpec();
    background_ = QPixmap();
    update();
}

void AgeSosChartWidget::setReferenceData(const QString& gender,
                                         int age,
                                         bool hasMeasurement,
                                         const QString& sosText)
{
    hasPatient_ = true;
    hasMeasurement_ = hasMeasurement;
    age_ = age;
    profile_ = profileFor(gender, age);
    spec_ = specFor(profile_);
    background_ = spec_.resourcePath.isEmpty() ? QPixmap() : QPixmap(spec_.resourcePath);

    bool ok = false;
    const double parsedSos = sosText.trimmed().toDouble(&ok);
    validSos_ = hasMeasurement && ok && std::isfinite(parsedSos);
    sos_ = validSos_ ? parsedSos : 0.0;
    update();
}

AgeSosChartWidget::Profile AgeSosChartWidget::profile() const
{
    return profile_;
}

AgeSosChartWidget::Profile AgeSosChartWidget::profileFor(const QString& gender, int age)
{
    if (age < 0 || age > 100) return Profile::None;

    const bool female = gender.contains(QStringLiteral("女"));
    const bool male = gender.contains(QStringLiteral("男"));
    if (!female && !male) return Profile::None;

    if (age < 20) return female ? Profile::Girl : Profile::Boy;
    return female ? Profile::Woman : Profile::Man;
}

AgeSosChartWidget::ChartSpec AgeSosChartWidget::specFor(Profile profile)
{
    switch (profile) {
    case Profile::Girl:
        return {profile, QStringLiteral(":/images/age_sos_girl.bmp"),
                0.0, 20.0, 3006.0, 4183.0, 43.0, 543.0, 64.0, 362.0};
    case Profile::Boy:
        return {profile, QStringLiteral(":/images/age_sos_boy.bmp"),
                0.0, 20.0, 2970.0, 4136.0, 40.0, 539.0, 45.0, 342.0};
    case Profile::Woman:
        return {profile, QStringLiteral(":/images/age_sos_woman.bmp"),
                20.0, 100.0, 3618.0, 4530.0, 46.0, 544.0, 55.0, 381.0};
    case Profile::Man:
        return {profile, QStringLiteral(":/images/age_sos_man.bmp"),
                20.0, 100.0, 3562.0, 4482.0, 46.0, 544.0, 55.0, 381.0};
    case Profile::None:
        break;
    }
    return {};
}

QRectF AgeSosChartWidget::fittedImageRect() const
{
    if (background_.isNull() || width() <= 0 || height() <= 0) return {};

    const QRectF available = rect().adjusted(6, 6, -6, -6);
    const QSizeF sourceSize = background_.size();
    const qreal scale = qMin(available.width() / sourceSize.width(),
                             available.height() / sourceSize.height());
    const QSizeF targetSize(sourceSize.width() * scale, sourceSize.height() * scale);
    return QRectF(available.center().x() - targetSize.width() / 2.0,
                  available.center().y() - targetSize.height() / 2.0,
                  targetSize.width(), targetSize.height());
}

bool AgeSosChartWidget::measurementPoint(QPointF* point) const
{
    if (!point || !hasMeasurement_ || !validSos_ || background_.isNull()) return false;
    if (age_ < spec_.minAge || age_ > spec_.maxAge ||
        sos_ < spec_.minSos || sos_ > spec_.maxSos) {
        return false;
    }

    const qreal ageRatio = (age_ - spec_.minAge) / (spec_.maxAge - spec_.minAge);
    const qreal sosRatio = (spec_.maxSos - sos_) / (spec_.maxSos - spec_.minSos);
    const QPointF imagePoint(spec_.xAtMinAge + ageRatio * (spec_.xAtMaxAge - spec_.xAtMinAge),
                             spec_.yAtMaxSos + sosRatio * (spec_.yAtMinSos - spec_.yAtMaxSos));

    const QRectF imageRect = fittedImageRect();
    point->setX(imageRect.left() + imagePoint.x() / background_.width() * imageRect.width());
    point->setY(imageRect.top() + imagePoint.y() / background_.height() * imageRect.height());
    return true;
}

void AgeSosChartWidget::drawStatus(QPainter* painter, const QString& text) const
{
    if (!painter || text.isEmpty()) return;

    QFont font(QStringLiteral("Microsoft YaHei"));
    font.setPixelSize(12);
    painter->setFont(font);
    const QFontMetrics metrics(font);
    const QSize textSize(metrics.horizontalAdvance(text) + 20, metrics.height() + 10);
    const QRect statusRect((width() - textSize.width()) / 2,
                           height() - textSize.height() - 10,
                           textSize.width(), textSize.height());
    painter->setPen(QColor(144, 147, 153));
    painter->setBrush(QColor(255, 255, 255, 230));
    painter->drawRoundedRect(statusRect, 4, 4);
    painter->drawText(statusRect, Qt::AlignCenter, text);
}

void AgeSosChartWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.fillRect(rect(), Qt::white);

    if (!hasPatient_) {
        painter.setPen(QColor(144, 147, 153));
        QFont font(QStringLiteral("Microsoft YaHei"));
        font.setPixelSize(14);
        painter.setFont(font);
        painter.drawText(rect(), Qt::AlignCenter, QStringLiteral("请选择患者"));
        return;
    }

    if (profile_ == Profile::None || background_.isNull()) {
        painter.setPen(QColor(144, 147, 153));
        QFont font(QStringLiteral("Microsoft YaHei"));
        font.setPixelSize(14);
        painter.setFont(font);
        painter.drawText(rect(), Qt::AlignCenter, QStringLiteral("患者性别或年龄信息不完整"));
        return;
    }

    const QRectF imageRect = fittedImageRect();
    painter.drawPixmap(imageRect, background_, QRectF(background_.rect()));

    if (!hasMeasurement_) {
        drawStatus(&painter, QStringLiteral("暂无检测结果"));
        return;
    }
    if (!validSos_) {
        drawStatus(&painter, QStringLiteral("检测结果数据无效"));
        return;
    }

    QPointF point;
    if (!measurementPoint(&point)) {
        drawStatus(&painter, QStringLiteral("结果超出参考图范围"));
        return;
    }

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawEllipse(point, 9, 9);
    painter.setBrush(QColor(245, 108, 108));
    painter.drawEllipse(point, 6, 6);

    const QString label = QStringLiteral("%1岁  %2 m/s").arg(age_).arg(sos_, 0, 'f', 1);
    QFont labelFont(QStringLiteral("Microsoft YaHei"));
    labelFont.setPixelSize(12);
    painter.setFont(labelFont);
    const QFontMetrics metrics(labelFont);
    const QSize labelSize(metrics.horizontalAdvance(label) + 16, metrics.height() + 8);
    int labelX = qRound(point.x()) + 10;
    if (labelX + labelSize.width() > imageRect.right() - 4) {
        labelX = qRound(point.x()) - labelSize.width() - 10;
    }
    int labelY = qRound(point.y()) - labelSize.height() - 8;
    const bool nearTopRightLegend = point.x() > imageRect.left() + imageRect.width() * 0.62 &&
                                    point.y() < imageRect.top() + imageRect.height() * 0.32;
    if (labelY < imageRect.top() + 4 || nearTopRightLegend) {
        labelY = qRound(point.y()) + 10;
    }
    if (labelY + labelSize.height() > imageRect.bottom() - 4) {
        labelY = qRound(point.y()) - labelSize.height() - 8;
    }
    const QRect labelRect(labelX, labelY, labelSize.width(), labelSize.height());

    painter.setPen(QColor(245, 108, 108));
    painter.setBrush(QColor(255, 255, 255, 235));
    painter.drawRoundedRect(labelRect, 4, 4);
    painter.drawText(labelRect, Qt::AlignCenter, label);
}
