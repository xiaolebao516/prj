#include "reportwidget.h"

#include <QPainter>
#include <QPaintEvent>

namespace {

constexpr int kReportWidth = 795;
constexpr int kReportHeight = 1124;
constexpr int kReportBodyCutY = 846;
constexpr int kTemplateFooterY = 1235;

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
    setMinimumSize(380, 540);
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
    return QSize(560, 792);
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
    painter->drawPixmap(QRect(0, 0, kReportWidth, kReportBodyCutY),
                        template_,
                        QRect(0, 0, template_.width(), kReportBodyCutY));
    painter->drawPixmap(QRect(0,
                              kReportBodyCutY,
                              kReportWidth,
                              kReportHeight - kReportBodyCutY),
                        template_,
                        QRect(0,
                              kTemplateFooterY,
                              template_.width(),
                              kReportHeight - kReportBodyCutY));
    // The source template contains a small isolated dark artifact in this blank area.
    painter->fillRect(QRectF(170, 822, 7, 7), Qt::white);

    QFont valueFont(QStringLiteral("Microsoft YaHei"));
    valueFont.setPixelSize(16);
    painter->setFont(valueFont);
    painter->setPen(QColor(65, 65, 65));

    const auto drawValue = [painter](const QRectF& rect, const QString& value) {
        painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, shown(value));
    };

    drawValue(QRectF(95, 299, 285, 28), data_.patientName);
    drawValue(QRectF(95, 332, 285, 28), data_.age);
    drawValue(QRectF(120, 366, 260, 28), data_.birthDay);
    drawValue(QRectF(95, 399, 285, 28), data_.height);

    drawValue(QRectF(445, 299, 305, 28), data_.patientId);
    drawValue(QRectF(465, 332, 285, 28), data_.gender);
    drawValue(QRectF(485, 366, 265, 28), data_.measuredAt);
    drawValue(QRectF(465, 399, 285, 28), data_.weight);

    drawValue(QRectF(125, 506, 255, 28), data_.part);
    drawValue(QRectF(95, 540, 285, 28), data_.sos);
    drawValue(QRectF(95, 573, 285, 28), data_.tScore);
    drawValue(QRectF(490, 506, 260, 28), data_.boneStrength);
    drawValue(QRectF(465, 540, 285, 28), data_.zScore);

    QFont diagnosisFont(QStringLiteral("Microsoft YaHei"));
    diagnosisFont.setPixelSize(15);
    painter->setFont(diagnosisFont);
    painter->drawText(QRectF(45, 907, 700, 82),
                      Qt::TextWordWrap | Qt::AlignLeft | Qt::AlignTop,
                      shown(data_.diagnosis));

    painter->setFont(valueFont);
    painter->drawText(QRectF(510, 995, 240, 34),
                      Qt::AlignLeft | Qt::AlignVCenter,
                      shown(data_.operatorName));
}
