#pragma once

// Paint-only widgets of the "Quiet Instrument" UI. None of them holds
// measurement or patient logic; MainWindow feeds them values it already has.

#include <QColor>
#include <QIcon>
#include <QList>
#include <QPixmap>
#include <QPointer>
#include <QStyledItemDelegate>
#include <QWidget>

#include <functional>

class QPainter;

namespace Icons {

enum class Glyph {
    Plug,
    Activity,
    Repeat,
    Folder,
    FileText,
    FileDown,
    Target,
    Help,
    Info,
    Search,
    Plus,
    Swap,
    Play,
    Stop,
    Download,
    Trash,
    Edit,
    Clock,
    ChevronLeft,
    Printer,
    Users,
    LogOut,
    Calendar,
};

// Stroke icon on a 24-unit grid, drawn as vectors at any size and DPI.
// Disabled mode uses `disabled` (ink-300 when not given).
QIcon icon(Glyph glyph, const QColor& color, const QColor& disabled = QColor());
void paint(QPainter* painter, const QRectF& rect, Glyph glyph, const QColor& color);

} // namespace Icons

// The ultrasound-wave product mark.
class BrandMark : public QWidget
{
public:
    explicit BrandMark(int size, QWidget* parent = nullptr);
    static void paint(QPainter* painter, const QRectF& rect);

protected:
    void paintEvent(QPaintEvent* event) override;
};

// Page background that also paints the soft shadow under each registered
// card. The shadow lives in the parent, so cards that redraw at 12 Hz
// (waveforms) never go through a QGraphicsEffect.
class CardCanvas : public QWidget
{
public:
    explicit CardCanvas(QWidget* parent = nullptr);
    void addCard(QWidget* card);

protected:
    void paintEvent(QPaintEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    QList<QPointer<QWidget>> cards_;
    QPixmap shadow_;
};

// Login page brand panel: deep ink with concentric ultrasound arcs.
class BrandPanel : public QWidget
{
public:
    explicit BrandPanel(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
};

// Round badge with the first character of a name.
class AvatarBadge : public QWidget
{
public:
    explicit AvatarBadge(int size, QWidget* parent = nullptr);
    void setName(const QString& name);
    static QPixmap render(const QString& name, int size, qreal dpr, bool emphasised);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QString name_;
};

// Horizontal T-score band (-5 .. +3: very low / low / normal) with a marker at
// the shown T value. Display only; NaN hides the marker.
class TScoreGauge : public QWidget
{
public:
    explicit TScoreGauge(QWidget* parent = nullptr);
    void setTScore(double tScore);
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    double t_;
};

// Archive name column: initial avatar, the name and a "当前" tag on the row
// that is the current subject. The cell text itself is unchanged.
class NameAvatarDelegate : public QStyledItemDelegate
{
public:
    NameAvatarDelegate(std::function<bool(const QModelIndex&)> isCurrent, QObject* parent = nullptr);
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
    std::function<bool(const QModelIndex&)> isCurrent_;
};

// Five short segments: finished rounds, the running round, the rest.
class RoundProgress : public QWidget
{
public:
    explicit RoundProgress(QWidget* parent = nullptr);
    void setProgress(int finished, bool running, int total);
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    int finished_ = 0;
    bool running_ = false;
    int total_ = 5;
};
