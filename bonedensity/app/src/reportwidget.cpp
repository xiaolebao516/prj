#include "reportwidget.h"

#include <QPainter>
#include <QPaintEvent>
#include <QDateTime>
#include <QStringList>

namespace {

constexpr int kReportWidth = 795;
constexpr int kReportHeight = 1124;
QString shown(const QString& value)
{
    return value.trimmed().isEmpty() ? QStringLiteral("--") : value;
}

} // namespace

ReportWidget::ReportWidget(QWidget* parent)
    : QWidget(parent)
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
    const QColor blue(25, 148, 216);
    const QColor dark(45, 49, 55);
    const QColor muted(126, 132, 140);
    const QColor line(210, 218, 228);
    const QColor red(245, 96, 96);

    painter->fillRect(QRect(0, 0, kReportWidth, kReportHeight), Qt::white);
    painter->setPen(QPen(blue, 8));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(QRectF(25, 20, 745, 1080));
    painter->fillRect(QRectF(29, 24, 737, 50), blue);

    QFont font(QStringLiteral("Microsoft YaHei"));
    font.setPixelSize(22);
    font.setBold(true);
    painter->setFont(font);
    painter->setPen(Qt::white);
    painter->drawText(QRectF(48, 28, 250, 38), Qt::AlignVCenter,
                      QStringLiteral("医疗参考"));

    painter->setBrush(QColor(242, 52, 66));
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(QRectF(244, 88, 50, 50), 12, 12);
    painter->fillRect(QRectF(264, 98, 10, 30), Qt::white);
    painter->fillRect(QRectF(254, 108, 30, 10), Qt::white);
    font.setPixelSize(28);
    painter->setFont(font);
    painter->setPen(dark);
    painter->drawText(QRectF(312, 88, 420, 52), Qt::AlignVCenter,
                      QStringLiteral("超声骨密度检测报告"));

    const auto sectionTitle = [&](int y, const QString& title) {
        QFont titleFont(QStringLiteral("Microsoft YaHei"));
        titleFont.setPixelSize(17);
        titleFont.setBold(true);
        painter->setFont(titleFont);
        painter->setPen(QColor(80, 84, 90));
        painter->drawText(QRectF(54, y, 680, 28), Qt::AlignVCenter, title);
    };
    const auto roundedPanel = [&](const QRectF& rect) {
        painter->setPen(QPen(line, 1));
        painter->setBrush(QColor(252, 253, 255));
        painter->drawRoundedRect(rect, 7, 7);
    };
    const auto labelValue = [&](const QRectF& rect, const QString& label,
                                const QString& value) {
        QFont small(QStringLiteral("Microsoft YaHei"));
        small.setPixelSize(13);
        painter->setFont(small);
        painter->setPen(muted);
        painter->drawText(QRectF(rect.x(), rect.y(), 82, rect.height()),
                          Qt::AlignLeft | Qt::AlignVCenter, label);
        painter->setPen(dark);
        painter->drawText(QRectF(rect.x() + 82, rect.y(), rect.width() - 82, rect.height()),
                          Qt::AlignLeft | Qt::AlignVCenter, shown(value));
    };

    sectionTitle(150, QStringLiteral("受检者信息"));
    roundedPanel(QRectF(54, 180, 687, 146));
    labelValue(QRectF(72, 190, 300, 28), QStringLiteral("姓名："), data_.patientName);
    labelValue(QRectF(400, 190, 320, 28), QStringLiteral("ID："), data_.patientId);
    labelValue(QRectF(72, 222, 300, 28), QStringLiteral("年龄："), shown(data_.age) + QStringLiteral(" 岁"));
    labelValue(QRectF(400, 222, 320, 28), QStringLiteral("性别："), data_.gender);
    labelValue(QRectF(72, 254, 300, 28), QStringLiteral("出生年月："), data_.birthDay);
    labelValue(QRectF(400, 254, 320, 28), QStringLiteral("检测时间："), data_.measuredAt);
    labelValue(QRectF(72, 286, 300, 28), QStringLiteral("身高："), data_.height);
    labelValue(QRectF(400, 286, 320, 28), QStringLiteral("体重："), data_.weight);

    sectionTitle(338, QStringLiteral("测量结果"));
    roundedPanel(QRectF(54, 368, 687, 78));
    const QStringList resultLabels = {QStringLiteral("桡骨 SOS"), QStringLiteral("骨强度"),
                                      QStringLiteral("T 值"), QStringLiteral("Z 值")};
    const QStringList resultValues = {data_.sos, data_.boneStrength,
                                      data_.tScore, data_.zScore};
    for (int i = 0; i < 4; ++i) {
        const qreal x = 70 + i * 168;
        if (i > 0) {
            painter->setPen(QPen(line, 1));
            painter->drawLine(QPointF(x - 14, 382), QPointF(x - 14, 431));
        }
        QFont labelFont(QStringLiteral("Microsoft YaHei"));
        labelFont.setPixelSize(11);
        painter->setFont(labelFont);
        painter->setPen(muted);
        painter->drawText(QRectF(x, 378, 145, 20), resultLabels.at(i));
        QFont valueFont(QStringLiteral("Microsoft YaHei"));
        valueFont.setPixelSize(i == 0 ? 19 : 16);
        valueFont.setBold(true);
        painter->setFont(valueFont);
        painter->setPen(i == 0 ? QColor(0, 133, 220) : dark);
        painter->drawText(QRectF(x, 399, 145, 30), Qt::AlignVCenter,
                          shown(resultValues.at(i)));
    }

    sectionTitle(458, QStringLiteral("年龄-SOS参考与历史测量"));
    roundedPanel(QRectF(54, 488, 687, 338));
    AgeSosChartWidget::renderChart(painter, QRectF(61, 496, 510, 318),
                                   data_.ageSosChart);

    painter->setPen(QPen(line, 1));
    painter->drawLine(QPointF(580, 503), QPointF(580, 810));
    QFont listTitle(QStringLiteral("Microsoft YaHei"));
    listTitle.setPixelSize(14);
    listTitle.setBold(true);
    painter->setFont(listTitle);
    painter->setPen(QColor(80, 84, 90));
    painter->drawText(QRectF(594, 504, 130, 24), QStringLiteral("历史记录"));

    const int visibleCount = qMin(6, int(data_.ageSosChart.points.size()));
    const int firstVisible = data_.ageSosChart.points.size() - visibleCount;
    for (int row = 0; row < visibleCount; ++row) {
        const AgeSosMeasurementPoint& point = data_.ageSosChart.points.at(firstVisible + row);
        const qreal y = 536 + row * 43;
        QFont rowFont(QStringLiteral("Microsoft YaHei"));
        rowFont.setPixelSize(11);
        painter->setFont(rowFont);
        painter->setPen(muted);
        painter->drawText(QRectF(594, y, 42, 18), QStringLiteral("%1岁").arg(point.age));
        rowFont.setPixelSize(13);
        rowFont.setBold(true);
        painter->setFont(rowFont);
        painter->setPen(point.highlighted ? red : dark);
        painter->drawText(QRectF(635, y - 2, 96, 22), Qt::AlignRight | Qt::AlignVCenter,
                          QStringLiteral("%1 m/s").arg(point.sos, 0, 'f', 0));
        QDateTime dateTime = QDateTime::fromString(point.measuredAt, Qt::ISODate);
        const QString date = dateTime.isValid()
            ? dateTime.date().toString(QStringLiteral("yyyy-MM-dd"))
            : point.measuredAt.left(10);
        rowFont.setPixelSize(9);
        rowFont.setBold(false);
        painter->setFont(rowFont);
        painter->setPen(muted);
        painter->drawText(QRectF(594, y + 18, 137, 17), Qt::AlignRight, date);
        painter->setPen(QPen(line, 1));
        painter->drawLine(QPointF(594, y + 39), QPointF(731, y + 39));
    }
    if (data_.ageSosChart.points.size() > visibleCount) {
        QFont note(QStringLiteral("Microsoft YaHei"));
        note.setPixelSize(9);
        painter->setFont(note);
        painter->setPen(muted);
        painter->drawText(QRectF(594, 790, 137, 18), Qt::AlignRight,
                          QStringLiteral("另有 %1 条更早记录")
                              .arg(data_.ageSosChart.points.size() - visibleCount));
    }

    sectionTitle(842, QStringLiteral("诊断提示"));
    roundedPanel(QRectF(54, 872, 687, 84));
    QFont diagnosisFont(QStringLiteral("Microsoft YaHei"));
    diagnosisFont.setPixelSize(13);
    painter->setFont(diagnosisFont);
    painter->setPen(QColor(90, 94, 100));
    painter->drawText(QRectF(70, 887, 655, 55),
                      Qt::TextWordWrap | Qt::AlignLeft | Qt::AlignTop,
                      shown(data_.diagnosis));

    painter->setPen(dark);
    painter->drawText(QRectF(505, 975, 220, 28), Qt::AlignVCenter,
                      QStringLiteral("检查医师：%1").arg(shown(data_.operatorName)));
    painter->setPen(QPen(QColor(100, 104, 110), 1));
    painter->drawLine(QPointF(54, 1030), QPointF(741, 1030));
    QFont footer(QStringLiteral("Microsoft YaHei"));
    footer.setPixelSize(10);
    painter->setFont(footer);
    painter->setPen(muted);
    painter->drawText(QRectF(54, 1038, 350, 24),
                      QStringLiteral("只做临床参考，不作证明材料"));
    painter->drawText(QRectF(490, 1038, 251, 24), Qt::AlignRight,
                      QStringLiteral("检测部位：%1").arg(shown(data_.part)));
}
