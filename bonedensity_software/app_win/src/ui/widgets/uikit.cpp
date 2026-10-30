#include "widgets/uikit.h"

#include "theme/theme.h"

#include <QApplication>
#include <QEvent>
#include <QFontMetrics>
#include <QFontMetricsF>
#include <QIconEngine>
#include <QImage>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QPolygonF>
#include <QStyle>
#include <QtWidgets/qdrawutil.h>

#include <cmath>
#include <initializer_list>
#include <utility>

// ============================== Icons ==============================

namespace Icons {
namespace {

void polyline(QPainter* p, std::initializer_list<QPointF> points)
{
    p->drawPolyline(QPolygonF(QList<QPointF>(points)));
}

void line(QPainter* p, qreal x1, qreal y1, qreal x2, qreal y2)
{
    p->drawLine(QPointF(x1, y1), QPointF(x2, y2));
}

void circle(QPainter* p, qreal cx, qreal cy, qreal r)
{
    p->drawEllipse(QPointF(cx, cy), r, r);
}

void dot(QPainter* p, qreal x, qreal y)
{
    p->drawLine(QPointF(x, y), QPointF(x + 0.01, y));
}

// Document outline with the folded corner, shared by FileText and FileDown.
void document(QPainter* p)
{
    QPainterPath body;
    body.moveTo(15, 2);
    body.lineTo(6, 2);
    body.quadTo(4, 2, 4, 4);
    body.lineTo(4, 20);
    body.quadTo(4, 22, 6, 22);
    body.lineTo(18, 22);
    body.quadTo(20, 22, 20, 20);
    body.lineTo(20, 7);
    body.closeSubpath();
    p->drawPath(body);
    QPainterPath fold;
    fold.moveTo(14, 2);
    fold.lineTo(14, 6);
    fold.quadTo(14, 8, 16, 8);
    fold.lineTo(20, 8);
    p->drawPath(fold);
}

class LineIconEngine : public QIconEngine
{
public:
    LineIconEngine(Glyph glyph, const QColor& normal, const QColor& disabled)
        : glyph_(glyph), normal_(normal), disabled_(disabled) {}

    void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State) override
    {
        Icons::paint(painter, rect, glyph_, mode == QIcon::Disabled ? disabled_ : normal_);
    }

    QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) override
    {
        QImage image(size, QImage::Format_ARGB32_Premultiplied);
        image.fill(Qt::transparent);
        QPainter painter(&image);
        paint(&painter, QRect(QPoint(0, 0), size), mode, state);
        painter.end();
        return QPixmap::fromImage(image);
    }

    QIconEngine* clone() const override { return new LineIconEngine(*this); }
    QString key() const override { return QStringLiteral("BoneDensityLineIcon"); }

private:
    Glyph glyph_;
    QColor normal_;
    QColor disabled_;
};

} // namespace

QIcon icon(Glyph glyph, const QColor& color, const QColor& disabled)
{
    return QIcon(new LineIconEngine(glyph, color,
                                    disabled.isValid() ? disabled : Theme::tokens().ink300));
}

void paint(QPainter* painter, const QRectF& rect, Glyph glyph, const QColor& color)
{
    if (!painter || rect.isEmpty()) return;
    const qreal side = qMin(rect.width(), rect.height());
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->translate(rect.center());
    painter->scale(side / 24.0, side / 24.0);
    painter->translate(-12.0, -12.0);
    // Small icons get a slightly heavier stroke so they stay legible.
    QPen pen(color, side <= 18 ? 2.1 : 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    switch (glyph) {
    case Glyph::Plug: {
        line(painter, 12, 22, 12, 17);
        line(painter, 9, 8, 9, 2);
        line(painter, 15, 8, 15, 2);
        QPainterPath body;
        body.moveTo(6, 8);
        body.lineTo(18, 8);
        body.lineTo(18, 13);
        body.quadTo(18, 17, 14, 17);
        body.lineTo(10, 17);
        body.quadTo(6, 17, 6, 13);
        body.closeSubpath();
        painter->drawPath(body);
        break;
    }
    case Glyph::Activity:
        polyline(painter, {{22, 12}, {18, 12}, {15, 21}, {9, 3}, {6, 12}, {2, 12}});
        break;
    case Glyph::Repeat: {
        polyline(painter, {{17, 2}, {21, 6}, {17, 10}});
        QPainterPath top;
        top.moveTo(3, 11);
        top.lineTo(3, 10);
        top.quadTo(3, 6, 7, 6);
        top.lineTo(21, 6);
        painter->drawPath(top);
        polyline(painter, {{7, 22}, {3, 18}, {7, 14}});
        QPainterPath bottom;
        bottom.moveTo(21, 13);
        bottom.lineTo(21, 14);
        bottom.quadTo(21, 18, 17, 18);
        bottom.lineTo(3, 18);
        painter->drawPath(bottom);
        break;
    }
    case Glyph::Folder: {
        QPainterPath folder;
        folder.moveTo(4, 20);
        folder.lineTo(20, 20);
        folder.quadTo(22, 20, 22, 18);
        folder.lineTo(22, 8);
        folder.quadTo(22, 6, 20, 6);
        folder.lineTo(12.1, 6);
        folder.quadTo(11, 6, 10.4, 5.1);
        folder.lineTo(9.6, 3.9);
        folder.quadTo(9, 3, 7.9, 3);
        folder.lineTo(4, 3);
        folder.quadTo(2, 3, 2, 5);
        folder.lineTo(2, 18);
        folder.quadTo(2, 20, 4, 20);
        folder.closeSubpath();
        painter->drawPath(folder);
        break;
    }
    case Glyph::FileText:
        document(painter);
        line(painter, 10, 9, 8, 9);
        line(painter, 16, 13, 8, 13);
        line(painter, 16, 17, 8, 17);
        break;
    case Glyph::FileDown:
        document(painter);
        line(painter, 12, 12, 12, 18);
        polyline(painter, {{9, 15}, {12, 18}, {15, 15}});
        break;
    case Glyph::Target:
        circle(painter, 12, 12, 9);
        circle(painter, 12, 12, 3);
        line(painter, 12, 3, 12, 6);
        line(painter, 12, 18, 12, 21);
        line(painter, 3, 12, 6, 12);
        line(painter, 18, 12, 21, 12);
        break;
    case Glyph::Help: {
        circle(painter, 12, 12, 10);
        QPainterPath mark;
        mark.moveTo(9.1, 9.0);
        mark.cubicTo(9.6, 6.6, 13.2, 6.0, 14.6, 8.0);
        mark.cubicTo(15.6, 9.6, 14.6, 11.0, 13.4, 11.6);
        mark.cubicTo(12.4, 12.1, 12.0, 12.6, 12.0, 13.6);
        painter->drawPath(mark);
        dot(painter, 12, 17);
        break;
    }
    case Glyph::Info:
        circle(painter, 12, 12, 10);
        line(painter, 12, 16, 12, 12);
        dot(painter, 12, 8);
        break;
    case Glyph::Search:
        circle(painter, 11, 11, 7.5);
        line(painter, 21, 21, 16.4, 16.4);
        break;
    case Glyph::Plus:
        line(painter, 12, 5, 12, 19);
        line(painter, 5, 12, 19, 12);
        break;
    case Glyph::Swap:
        polyline(painter, {{8, 3}, {4, 7}, {8, 11}});
        line(painter, 4, 7, 20, 7);
        polyline(painter, {{16, 21}, {20, 17}, {16, 13}});
        line(painter, 20, 17, 4, 17);
        break;
    case Glyph::Play: {
        painter->setBrush(color);
        QPolygonF triangle({QPointF(7, 4), QPointF(19, 12), QPointF(7, 20)});
        painter->drawPolygon(triangle);
        break;
    }
    case Glyph::Stop:
        painter->setPen(Qt::NoPen);
        painter->setBrush(color);
        painter->drawRoundedRect(QRectF(5, 5, 14, 14), 2.5, 2.5);
        break;
    case Glyph::Download: {
        QPainterPath tray;
        tray.moveTo(21, 15);
        tray.lineTo(21, 19);
        tray.quadTo(21, 21, 19, 21);
        tray.lineTo(5, 21);
        tray.quadTo(3, 21, 3, 19);
        tray.lineTo(3, 15);
        painter->drawPath(tray);
        polyline(painter, {{7, 10}, {12, 15}, {17, 10}});
        line(painter, 12, 15, 12, 3);
        break;
    }
    case Glyph::Trash: {
        line(painter, 3, 6, 21, 6);
        QPainterPath bin;
        bin.moveTo(19, 6);
        bin.lineTo(19, 20);
        bin.quadTo(19, 22, 17, 22);
        bin.lineTo(7, 22);
        bin.quadTo(5, 22, 5, 20);
        bin.lineTo(5, 6);
        painter->drawPath(bin);
        QPainterPath lid;
        lid.moveTo(8, 6);
        lid.lineTo(8, 4);
        lid.quadTo(8, 2, 10, 2);
        lid.lineTo(14, 2);
        lid.quadTo(16, 2, 16, 4);
        lid.lineTo(16, 6);
        painter->drawPath(lid);
        break;
    }
    case Glyph::Edit: {
        QPainterPath pencil;
        pencil.moveTo(17, 3);
        pencil.cubicTo(18.1, 1.9, 19.9, 1.9, 21, 3);
        pencil.cubicTo(22.1, 4.1, 22.1, 5.9, 21, 7);
        pencil.lineTo(7.5, 20.5);
        pencil.lineTo(2, 22);
        pencil.lineTo(3.5, 16.5);
        pencil.closeSubpath();
        painter->drawPath(pencil);
        break;
    }
    case Glyph::Clock:
        circle(painter, 12, 12, 10);
        polyline(painter, {{12, 6}, {12, 12}, {16, 14}});
        break;
    case Glyph::ChevronLeft:
        polyline(painter, {{15, 18}, {9, 12}, {15, 6}});
        break;
    case Glyph::Printer: {
        polyline(painter, {{6, 9}, {6, 2}, {18, 2}, {18, 9}});
        QPainterPath body;
        body.moveTo(6, 18);
        body.lineTo(4, 18);
        body.quadTo(2, 18, 2, 16);
        body.lineTo(2, 11);
        body.quadTo(2, 9, 4, 9);
        body.lineTo(20, 9);
        body.quadTo(22, 9, 22, 11);
        body.lineTo(22, 16);
        body.quadTo(22, 18, 20, 18);
        body.lineTo(18, 18);
        painter->drawPath(body);
        painter->drawRect(QRectF(6, 14, 12, 8));
        break;
    }
    case Glyph::Users: {
        circle(painter, 9, 7, 4);
        QPainterPath body;
        body.moveTo(16, 21);
        body.lineTo(16, 19);
        body.quadTo(16, 15, 12, 15);
        body.lineTo(6, 15);
        body.quadTo(2, 15, 2, 19);
        body.lineTo(2, 21);
        painter->drawPath(body);
        QPainterPath second;
        second.moveTo(22, 21);
        second.lineTo(22, 19);
        second.quadTo(22, 15.6, 19, 15.1);
        painter->drawPath(second);
        QPainterPath head;
        head.moveTo(16, 3.1);
        head.cubicTo(18.4, 3.7, 19.6, 6.4, 18.4, 8.7);
        head.cubicTo(17.9, 9.7, 17.0, 10.5, 16, 10.9);
        painter->drawPath(head);
        break;
    }
    case Glyph::LogOut: {
        QPainterPath door;
        door.moveTo(9, 21);
        door.lineTo(5, 21);
        door.quadTo(3, 21, 3, 19);
        door.lineTo(3, 5);
        door.quadTo(3, 3, 5, 3);
        door.lineTo(9, 3);
        painter->drawPath(door);
        polyline(painter, {{16, 17}, {21, 12}, {16, 7}});
        line(painter, 21, 12, 9, 12);
        break;
    }
    case Glyph::Calendar:
        painter->drawRoundedRect(QRectF(3, 4, 18, 18), 2, 2);
        line(painter, 16, 2, 16, 6);
        line(painter, 8, 2, 8, 6);
        line(painter, 3, 10, 21, 10);
        break;
    }
    painter->restore();
}

} // namespace Icons

// ============================== BrandMark ==============================

BrandMark::BrandMark(int size, QWidget* parent)
    : QWidget(parent)
{
    setFixedSize(size, size);
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

void BrandMark::paint(QPainter* painter, const QRectF& rect)
{
    const Theme::Tokens& t = Theme::tokens();
    const qreal s = qMin(rect.width(), rect.height()) / 28.0;
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->translate(rect.topLeft());
    painter->scale(s, s);
    painter->setPen(Qt::NoPen);
    painter->setBrush(t.accent);
    painter->drawRoundedRect(QRectF(0, 0, 28, 28), 8, 8);
    painter->setBrush(Qt::white);
    painter->drawEllipse(QPointF(7.5, 14), 1.8, 1.8);
    const qreal opacity[3] = {1.0, 0.72, 0.42};
    for (int i = 0; i < 3; ++i) {
        const qreal r = 4.0 * (i + 1);
        QColor c(Qt::white);
        c.setAlphaF(opacity[i]);
        painter->setPen(QPen(c, 1.8, Qt::SolidLine, Qt::RoundCap));
        painter->setBrush(Qt::NoBrush);
        painter->drawArc(QRectF(7.5 - r, 14 - r, 2 * r, 2 * r), 50 * 16, -100 * 16);
    }
    painter->restore();
}

void BrandMark::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    paint(&painter, rect());
}

// ============================== CardCanvas ==============================

namespace {

constexpr int kShadowBlur = 8;
constexpr int kShadowRadius = 12;
constexpr int kShadowOffsetY = 2;

void boxBlur(QImage& image, int radius)
{
    const int w = image.width();
    const int h = image.height();
    QImage temp(image.size(), image.format());
    const auto blurLine = [radius](const QRgb* src, QRgb* dst, int count, int stride) {
        for (int i = 0; i < count; ++i) {
            int a = 0, r = 0, g = 0, b = 0, n = 0;
            for (int k = -radius; k <= radius; ++k) {
                const int j = qBound(0, i + k, count - 1);
                const QRgb px = src[j * stride];
                a += qAlpha(px); r += qRed(px); g += qGreen(px); b += qBlue(px);
                ++n;
            }
            dst[i * stride] = qRgba(r / n, g / n, b / n, a / n);
        }
    };
    for (int y = 0; y < h; ++y) {
        blurLine(reinterpret_cast<const QRgb*>(image.constScanLine(y)),
                 reinterpret_cast<QRgb*>(temp.scanLine(y)), w, 1);
    }
    const int stride = int(temp.bytesPerLine() / sizeof(QRgb));
    for (int x = 0; x < w; ++x) {
        blurLine(reinterpret_cast<const QRgb*>(temp.constBits()) + x,
                 reinterpret_cast<QRgb*>(image.bits()) + x, h, stride);
    }
}

QPixmap makeShadow()
{
    const int core = 2 * kShadowRadius + 2;
    const int size = core + 2 * kShadowBlur;
    QImage image(size, size, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);
    {
        QPainter painter(&image);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(16, 24, 40, 30));
        painter.drawRoundedRect(QRectF(kShadowBlur, kShadowBlur, core, core), kShadowRadius, kShadowRadius);
    }
    for (int pass = 0; pass < 3; ++pass) boxBlur(image, kShadowBlur / 3 + 1);
    return QPixmap::fromImage(image);
}

} // namespace

CardCanvas::CardCanvas(QWidget* parent)
    : QWidget(parent), shadow_(makeShadow())
{
}

void CardCanvas::addCard(QWidget* card)
{
    if (!card) return;
    cards_.append(card);
    card->installEventFilter(this);
    update();
}

bool CardCanvas::eventFilter(QObject* watched, QEvent* event)
{
    switch (event->type()) {
    case QEvent::Move:
    case QEvent::Resize:
    case QEvent::Show:
    case QEvent::Hide:
        update();
        break;
    default:
        break;
    }
    return QWidget::eventFilter(watched, event);
}

void CardCanvas::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.fillRect(event->rect(), Theme::tokens().canvas);
    const int margin = kShadowBlur + kShadowRadius + 1;
    for (const QPointer<QWidget>& card : cards_) {
        if (!card || !card->isVisible()) continue;
        const QRect box(card->mapTo(this, QPoint(0, 0)), card->size());
        const QRect target = box.adjusted(-kShadowBlur, -kShadowBlur + kShadowOffsetY,
                                          kShadowBlur, kShadowBlur + kShadowOffsetY);
        if (!target.intersects(event->rect())) continue;
        qDrawBorderPixmap(&painter, target, QMargins(margin, margin, margin, margin), shadow_);
    }
}

// ============================== BrandPanel ==============================

BrandPanel::BrandPanel(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
}

void BrandPanel::paintEvent(QPaintEvent*)
{
    const Theme::Tokens& t = Theme::tokens();
    QPainter painter(this);
    painter.fillRect(rect(), t.brandInk);
    painter.setRenderHint(QPainter::Antialiasing, true);
    // Concentric wavefronts leaving a transducer near the lower left.
    const QPointF origin(width() * 0.10, height() * 0.72);
    const QColor wave(0x5D, 0x86, 0xF0);
    const qreal step = qMax<qreal>(90.0, height() * 0.11);
    for (int i = 1; i <= 7; ++i) {
        const qreal r = step * i;
        QColor c = wave;
        c.setAlphaF(qMax(0.05, 0.6 * std::pow(0.68, i - 1)));
        painter.setPen(QPen(c, 1.5));
        painter.drawArc(QRectF(origin.x() - r, origin.y() - r, 2 * r, 2 * r), 48 * 16, -96 * 16);
    }
    painter.setPen(Qt::NoPen);
    painter.setBrush(wave);
    painter.drawEllipse(origin, 3.0, 3.0);
}

// ============================== AvatarBadge ==============================

AvatarBadge::AvatarBadge(int size, QWidget* parent)
    : QWidget(parent)
{
    setFixedSize(size, size);
}

void AvatarBadge::setName(const QString& name)
{
    if (name_ == name) return;
    name_ = name;
    update();
}

QPixmap AvatarBadge::render(const QString& name, int size, qreal dpr, bool emphasised)
{
    const Theme::Tokens& t = Theme::tokens();
    QPixmap pixmap(QSize(size, size) * dpr);
    pixmap.setDevicePixelRatio(dpr);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(emphasised ? t.accentSoft : t.fill);
    painter.drawEllipse(QRectF(0, 0, size, size));
    const QString initial = name.trimmed().left(1).toUpper();
    if (initial.isEmpty()) {
        Icons::paint(&painter, QRectF(size * 0.28, size * 0.28, size * 0.44, size * 0.44),
                     Icons::Glyph::Users, t.ink400);
    } else {
        painter.setPen(emphasised ? t.accent : t.ink700);
        painter.setFont(Theme::uiFont(qRound(size * 0.42), QFont::DemiBold));
        painter.drawText(QRectF(0, 0, size, size), Qt::AlignCenter, initial);
    }
    return pixmap;
}

void AvatarBadge::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, render(name_, width(), devicePixelRatioF(), !name_.isEmpty()));
}

// ============================== TScoreGauge ==============================

namespace {
constexpr double kGaugeMinT = -5.0;
constexpr double kGaugeMaxT = 3.0;
}

TScoreGauge::TScoreGauge(QWidget* parent)
    : QWidget(parent), t_(std::nan(""))
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFixedHeight(40);
}

void TScoreGauge::setTScore(double tScore)
{
    if ((std::isnan(t_) && std::isnan(tScore)) || t_ == tScore) return;
    t_ = tScore;
    update();
}

QSize TScoreGauge::sizeHint() const { return {240, 40}; }
QSize TScoreGauge::minimumSizeHint() const { return {120, 40}; }

void TScoreGauge::paintEvent(QPaintEvent*)
{
    const Theme::Tokens& t = Theme::tokens();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    const qreal left = 1.0;
    const qreal right = width() - 1.0;
    const qreal span = right - left;
    const auto x = [&](double value) {
        return left + (qBound(kGaugeMinT, value, kGaugeMaxT) - kGaugeMinT) / (kGaugeMaxT - kGaugeMinT) * span;
    };
    const bool hasValue = std::isfinite(t_);
    const qreal barTop = 17.0;
    const qreal barHeight = 6.0;
    struct Segment { double from; double to; QColor color; };
    const Segment segments[3] = {
        {kGaugeMinT, -2.5, QColor(0xF5, 0xCF, 0xC9)},
        {-2.5, -1.0, QColor(0xF8, 0xDF, 0xAF)},
        {-1.0, kGaugeMaxT, QColor(0xBF, 0xE5, 0xD0)}};
    painter.setOpacity(hasValue ? 1.0 : 0.5);
    painter.setPen(Qt::NoPen);
    for (int i = 0; i < 3; ++i) {
        qreal x0 = x(segments[i].from) + (i == 0 ? 0 : 1);
        qreal x1 = x(segments[i].to) - (i == 2 ? 0 : 1);
        QPainterPath path;
        path.addRoundedRect(QRectF(x0, barTop, x1 - x0, barHeight), 3, 3);
        painter.fillPath(path, segments[i].color);
    }
    painter.setOpacity(1.0);

    painter.setFont(Theme::numberFont(10, QFont::Normal));
    painter.setPen(t.ink400);
    const qreal labelTop = barTop + barHeight + 3;
    const QFontMetricsF fm(painter.font());
    const auto label = [&](double value, const QString& text, Qt::Alignment align) {
        const qreal w = fm.horizontalAdvance(text) + 2;
        qreal lx = x(value) - w / 2;
        if (align & Qt::AlignLeft) lx = left;
        if (align & Qt::AlignRight) lx = right - w;
        painter.drawText(QRectF(lx, labelTop, w, fm.height()), Qt::AlignCenter, text);
    };
    label(kGaugeMinT, QStringLiteral("−5"), Qt::AlignLeft);
    label(-2.5, QStringLiteral("−2.5"), Qt::AlignHCenter);
    label(-1.0, QStringLiteral("−1"), Qt::AlignHCenter);
    label(kGaugeMaxT, QStringLiteral("+3"), Qt::AlignRight);

    if (!hasValue) return;
    const qreal mx = x(t_);
    painter.setPen(Qt::NoPen);
    painter.setBrush(t.ink900);
    painter.drawRoundedRect(QRectF(mx - 1.0, 12.0, 2.0, barTop + barHeight - 12.0 + 3.0), 1, 1);
    const QString text = QStringLiteral("T %1").arg(t_ < 0 ? QStringLiteral("−%1").arg(-t_, 0, 'f', 2)
                                                           : QString::number(t_, 'f', 2));
    painter.setFont(Theme::numberFont(11, QFont::Bold));
    const QFontMetricsF bold(painter.font());
    const qreal w = bold.horizontalAdvance(text) + 2;
    const qreal lx = qBound(left, mx - w / 2, right - w);
    painter.setPen(t.ink900);
    painter.drawText(QRectF(lx, 0, w, 12), Qt::AlignCenter, text);
}

// ============================== NameAvatarDelegate ==============================

namespace {
constexpr int kRowAvatar = 28;
const QString kCurrentTag = QStringLiteral("当前");
}

NameAvatarDelegate::NameAvatarDelegate(std::function<bool(const QModelIndex&)> isCurrent, QObject* parent)
    : QStyledItemDelegate(parent), isCurrent_(std::move(isCurrent))
{
}

void NameAvatarDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                               const QModelIndex& index) const
{
    const Theme::Tokens& t = Theme::tokens();
    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);
    const QString name = opt.text;
    opt.text.clear();
    const QWidget* widget = opt.widget;
    QStyle* style = widget ? widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);

    const QRect area = style->subElementRect(QStyle::SE_ItemViewItemText, &opt, widget);
    const bool current = isCurrent_ && isCurrent_(index);
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    const qreal dpr = painter->device() ? painter->device()->devicePixelRatioF() : 1.0;
    const QRect avatar(area.left(), area.center().y() - kRowAvatar / 2 + 1, kRowAvatar, kRowAvatar);
    painter->drawPixmap(avatar.topLeft(), AvatarBadge::render(name, kRowAvatar, dpr, current));

    QFont font = opt.font;
    font.setWeight(current ? QFont::DemiBold : QFont::Normal);
    painter->setFont(font);
    const QFontMetrics fm(font);
    QFont tagFont = Theme::uiFont(11, QFont::DemiBold);
    const QFontMetrics tagMetrics(tagFont);
    const int tagWidth = current ? tagMetrics.horizontalAdvance(kCurrentTag) + 12 : 0;
    const int textLeft = avatar.right() + 11;
    const int available = qMax(0, area.right() - textLeft - (current ? tagWidth + 8 : 0));
    const QString shown = fm.elidedText(name, Qt::ElideRight, available);
    painter->setPen(t.ink900);
    painter->drawText(QRect(textLeft, area.top(), available, area.height()), Qt::AlignLeft | Qt::AlignVCenter, shown);
    if (current) {
        const QRect tag(textLeft + fm.horizontalAdvance(shown) + 8, area.center().y() - 9, tagWidth, 18);
        painter->setPen(QPen(t.accentSoftBorder, 1));
        painter->setBrush(t.surface);
        painter->drawRoundedRect(QRectF(tag).adjusted(0.5, 0.5, -0.5, -0.5), 4, 4);
        painter->setFont(tagFont);
        painter->setPen(t.accentInk);
        painter->drawText(tag, Qt::AlignCenter, kCurrentTag);
    }
    painter->restore();
}

QSize NameAvatarDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.rwidth() += kRowAvatar + 11 + 48;
    size.setHeight(qMax(size.height(), kRowAvatar + 8));
    return size;
}

// ============================== RoundProgress ==============================

RoundProgress::RoundProgress(QWidget* parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFixedSize(sizeHint());
}

void RoundProgress::setProgress(int finished, bool running, int total)
{
    total = qMax(1, total);
    finished = qBound(0, finished, total);
    if (finished == finished_ && running == running_ && total == total_) return;
    finished_ = finished;
    running_ = running;
    total_ = total;
    setFixedSize(sizeHint());
    update();
}

QSize RoundProgress::sizeHint() const
{
    return {total_ * 18 + (total_ - 1) * 5, 10};
}

void RoundProgress::paintEvent(QPaintEvent*)
{
    const Theme::Tokens& t = Theme::tokens();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    for (int i = 0; i < total_; ++i) {
        const QColor color = i < finished_ ? t.okDot
                             : (i == finished_ && running_) ? t.accent : t.line;
        painter.setBrush(color);
        painter.drawRoundedRect(QRectF(i * 23.0, 2.0, 18.0, 6.0), 3, 3);
    }
}
