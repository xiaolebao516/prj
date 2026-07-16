#include "reportwidget.h"

#include <QPainter>
#include <QPaintEvent>

namespace {

constexpr int kReportWidth = 795;
constexpr int kReportHeight = 1513;

QString shown(const QString& value)
{
    return value.trimmed().isEmpty() ? QStringLiteral("--") : value;
}

} // namespace

ReportWidget::ReportWidget(QWidget* parent)
    : QWidget(parent),
      template_(QStringLiteral(":/images/report.bmp"))
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(380, 620);
}

void ReportWidget::setReportData(const ReportData& data)
{
    data_ = data;
    update();
}

const ReportData& ReportWidget::reportData() const
{
    return data_;
}

QSize ReportWidget::sizeHint() const
{
    return QSize(540, 900);
}

void ReportWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.fillRect(rect(), QColor(235, 238, 242));
    renderReport(&painter, rect());
}

void ReportWidget::renderReport(QPainter* painter, const QRectF& targetRect) const
{
    if (!painter || targetRect.isEmpty()) return;

    const qreal scale = qMin(targetRect.width() / kReportWidth,
                             targetRect.height() / kReportHeight);
    const QSizeF pageSize(kReportWidth * scale, kReportHeight * scale);
    const QRectF pageRect(targetRect.center().x() - pageSize.width() / 2.0,
                          targetRect.center().y() - pageSize.height() / 2.0,
                          pageSize.width(), pageSize.height());

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter->translate(pageRect.topLeft());
    painter->scale(scale, scale);
    drawLogicalPage(painter);
    painter->restore();
}

void ReportWidget::drawLogicalPage(QPainter* painter) const
{
    painter->drawPixmap(QRect(0, 0, kReportWidth, kReportHeight), template_);

    QFont valueFont(QStringLiteral("Microsoft YaHei"));
    valueFont.setPixelSize(14);
    painter->setFont(valueFont);
    painter->setPen(QColor(65, 65, 65));

    painter->drawText(QPointF(95, 319), shown(data_.patientName));
    painter->drawText(QPointF(95, 352), shown(data_.age));
    painter->drawText(QPointF(112, 386), shown(data_.birthDay));
    painter->drawText(QPointF(95, 419), shown(data_.height));

    painter->drawText(QPointF(445, 319), shown(data_.patientId));
    painter->drawText(QPointF(465, 352), shown(data_.gender));
    painter->drawText(QPointF(485, 386), shown(data_.measuredAt));
    painter->drawText(QPointF(465, 419), shown(data_.weight));

    painter->drawText(QPointF(125, 526), shown(data_.part));
    painter->drawText(QPointF(95, 560), shown(data_.sos));
    painter->drawText(QPointF(95, 593), shown(data_.tScore));
    painter->drawText(QPointF(490, 526), shown(data_.boneStrength));
    painter->drawText(QPointF(465, 560), shown(data_.zScore));

    QFont diagnosisFont(QStringLiteral("Microsoft YaHei"));
    diagnosisFont.setPixelSize(13);
    painter->setFont(diagnosisFont);
    painter->drawText(QRectF(45, 1300, 700, 78),
                      Qt::TextWordWrap | Qt::AlignLeft | Qt::AlignTop,
                      shown(data_.diagnosis));

    painter->setFont(valueFont);
    painter->drawText(QRectF(500, 1390, 260, 34),
                      Qt::AlignLeft | Qt::AlignVCenter,
                      shown(data_.operatorName));
}
