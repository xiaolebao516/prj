#include "theme.h"

#include <QApplication>
#include <QFile>
#include <QPalette>
#include <QPen>
#include <QStyle>
#include <QStyleFactory>
#include <QVariant>
#include <QWidget>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>

#include <algorithm>
#include <utility>
#include <vector>

namespace Theme {

const Tokens& tokens()
{
    static const Tokens instance;
    return instance;
}

QString uiFamily()
{
    return QStringLiteral("Microsoft YaHei UI");
}

QFont uiFont(int pixelSize, QFont::Weight weight)
{
    QFont font(uiFamily());
    font.setFamilies({uiFamily(), QStringLiteral("Microsoft YaHei"), QStringLiteral("Segoe UI")});
    font.setPixelSize(pixelSize);
    font.setWeight(weight);
    return font;
}

QFont numberFont(int pixelSize, QFont::Weight weight)
{
    QFont font(QStringLiteral("Segoe UI"));
    font.setFamilies({QStringLiteral("Segoe UI"), uiFamily(), QStringLiteral("Microsoft YaHei")});
    font.setPixelSize(pixelSize);
    font.setWeight(weight);
    return font;
}

QFont monoFont(int pixelSize)
{
    QFont font(QStringLiteral("Consolas"));
    font.setFamilies({QStringLiteral("Consolas"), QStringLiteral("Cascadia Mono"), uiFamily()});
    font.setStyleHint(QFont::Monospace);
    font.setPixelSize(pixelSize);
    return font;
}

void installApplicationStyle()
{
    static bool installed = false;
    if (installed || !qApp) return;
    installed = true;

    // Fusion draws every control from the palette, so anything the QSS does
    // not cover (calendar pop-ups, message boxes, check marks) still matches.
    if (QStyle* fusion = QStyleFactory::create(QStringLiteral("Fusion"))) {
        QApplication::setStyle(fusion);
    }

    const Tokens& t = tokens();
    QPalette palette;
    palette.setColor(QPalette::Window, t.canvas);
    palette.setColor(QPalette::WindowText, t.ink900);
    palette.setColor(QPalette::Base, t.surface);
    palette.setColor(QPalette::AlternateBase, t.sunken);
    palette.setColor(QPalette::Text, t.ink900);
    palette.setColor(QPalette::Button, t.surface);
    palette.setColor(QPalette::ButtonText, t.ink900);
    palette.setColor(QPalette::BrightText, Qt::white);
    palette.setColor(QPalette::Highlight, t.accent);
    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::Link, t.accent);
    palette.setColor(QPalette::ToolTipBase, t.ink900);
    palette.setColor(QPalette::ToolTipText, Qt::white);
    palette.setColor(QPalette::PlaceholderText, t.ink400);
    palette.setColor(QPalette::Light, t.surface);
    palette.setColor(QPalette::Midlight, t.lineSoft);
    palette.setColor(QPalette::Mid, t.line);
    palette.setColor(QPalette::Dark, t.lineStrong);
    palette.setColor(QPalette::Shadow, t.ink400);
    for (const QPalette::ColorRole role : {QPalette::WindowText, QPalette::Text, QPalette::ButtonText}) {
        palette.setColor(QPalette::Disabled, role, t.ink300);
    }
    palette.setColor(QPalette::Disabled, QPalette::Base, t.canvas);
    palette.setColor(QPalette::Disabled, QPalette::Button, t.canvas);
    palette.setColor(QPalette::Disabled, QPalette::Highlight, t.lineStrong);
    QApplication::setPalette(palette);
    QApplication::setFont(uiFont(14));
}

QString styleSheet()
{
    QFile file(QStringLiteral(":/theme.qss"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return QString();
    QString sheet = QString::fromUtf8(file.readAll());

    const Tokens& t = tokens();
    std::vector<std::pair<QString, QColor>> map = {
        {QStringLiteral("canvas"), t.canvas},
        {QStringLiteral("surface"), t.surface},
        {QStringLiteral("sunken"), t.sunken},
        {QStringLiteral("fillHover"), t.fillHover},
        {QStringLiteral("fill"), t.fill},
        {QStringLiteral("lineSoft"), t.lineSoft},
        {QStringLiteral("lineStrong"), t.lineStrong},
        {QStringLiteral("line"), t.line},
        {QStringLiteral("ink900"), t.ink900},
        {QStringLiteral("ink700"), t.ink700},
        {QStringLiteral("ink500"), t.ink500},
        {QStringLiteral("ink400"), t.ink400},
        {QStringLiteral("ink300"), t.ink300},
        {QStringLiteral("accentHover"), t.accentHover},
        {QStringLiteral("accentPressed"), t.accentPressed},
        {QStringLiteral("accentSoftHover"), t.accentSoftHover},
        {QStringLiteral("accentSoftBorder"), t.accentSoftBorder},
        {QStringLiteral("accentSoft"), t.accentSoft},
        {QStringLiteral("accentInk"), t.accentInk},
        {QStringLiteral("accentTrack"), t.accentTrack},
        {QStringLiteral("accent"), t.accent},
        {QStringLiteral("okSoft"), t.okSoft},
        {QStringLiteral("okDot"), t.okDot},
        {QStringLiteral("ok"), t.ok},
        {QStringLiteral("warnSoft"), t.warnSoft},
        {QStringLiteral("warnDot"), t.warnDot},
        {QStringLiteral("warn"), t.warn},
        {QStringLiteral("badSoft"), t.badSoft},
        {QStringLiteral("badBorder"), t.badBorder},
        {QStringLiteral("badDot"), t.badDot},
        {QStringLiteral("bad"), t.bad},
    };
    const QString channelNames[4] = {QStringLiteral("A"), QStringLiteral("B"), QStringLiteral("C"), QStringLiteral("D")};
    for (int i = 0; i < 4; ++i) {
        map.push_back({QStringLiteral("ch%1Soft").arg(channelNames[i]), t.channelSoft[i]});
        map.push_back({QStringLiteral("ch%1Ink").arg(channelNames[i]), t.channelInk[i]});
    }
    // Longest names first so "@accentSoft" is not eaten by "@accent".
    std::sort(map.begin(), map.end(), [](const auto& a, const auto& b) {
        return a.first.size() > b.first.size();
    });
    for (const auto& [name, color] : map) {
        sheet.replace(QLatin1Char('@') + name, color.name(QColor::HexRgb).toUpper());
    }
    return sheet;
}

void repolish(QWidget* widget)
{
    if (!widget) return;
    widget->style()->unpolish(widget);
    widget->style()->polish(widget);
    widget->update();
}

void setTone(QWidget* widget, Tone tone)
{
    if (!widget) return;
    static const char* const names[] = {"", "info", "ok", "warn", "bad", "muted"};
    const QString value = QString::fromLatin1(names[int(tone)]);
    if (widget->property("tone").toString() == value) return;
    widget->setProperty("tone", value);
    repolish(widget);
}

void styleChart(QChart* chart)
{
    if (!chart) return;
    const Tokens& t = tokens();
    chart->legend()->hide();
    chart->setBackgroundVisible(false);
    chart->setBackgroundRoundness(0);
    chart->setPlotAreaBackgroundBrush(t.sunken);
    chart->setPlotAreaBackgroundPen(QPen(t.lineSoft, 1));
    chart->setPlotAreaBackgroundVisible(true);
    chart->setDropShadowEnabled(false);
}

void styleValueAxis(QValueAxis* axis, bool labelsVisible)
{
    if (!axis) return;
    const Tokens& t = tokens();
    axis->setTitleText(QString());
    axis->setLineVisible(false);
    axis->setGridLineVisible(true);
    QPen grid(t.line, 1, Qt::CustomDashLine);
    grid.setDashPattern({3.0, 4.0});
    axis->setGridLinePen(grid);
    axis->setMinorGridLineVisible(false);
    axis->setShadesVisible(false);
    axis->setLabelsVisible(labelsVisible);
    axis->setLabelsColor(t.ink400);
    axis->setLabelsFont(numberFont(10, QFont::Normal));
}

} // namespace Theme
