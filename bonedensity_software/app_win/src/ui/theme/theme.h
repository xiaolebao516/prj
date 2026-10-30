#pragma once

// "Quiet Instrument" visual system (docs/design/2026-10-09-quiet-instrument-ui.md).
// Every colour the UI uses comes from Theme::tokens(); resources/theme.qss refers
// to the same values as @tokens, which styleSheet() substitutes at load time.

#include <QColor>
#include <QFont>
#include <QString>

class QWidget;
class QChart;
class QValueAxis;

namespace Theme {

struct Tokens {
    // Neutrals
    QColor canvas{0xF4, 0xF5, 0xF7};
    QColor surface{0xFF, 0xFF, 0xFF};
    QColor sunken{0xFA, 0xFB, 0xFC};
    QColor fill{0xEF, 0xF1, 0xF4};
    QColor fillHover{0xE6, 0xE9, 0xED};
    QColor line{0xE6, 0xE8, 0xEC};
    QColor lineSoft{0xEE, 0xF0, 0xF3};
    QColor lineStrong{0xD9, 0xDD, 0xE3};
    QColor ink900{0x15, 0x19, 0x1F};
    QColor ink700{0x3B, 0x43, 0x50};
    QColor ink500{0x66, 0x70, 0x85};
    QColor ink400{0x8A, 0x93, 0xA0};
    QColor ink300{0xA0, 0xA8, 0xB4};
    QColor brandInk{0x0F, 0x16, 0x23};       // login brand panel

    // Interaction
    QColor accent{0x2B, 0x5B, 0xD7};
    QColor accentHover{0x23, 0x49, 0xB4};
    QColor accentPressed{0x1E, 0x3F, 0x9B};
    QColor accentSoft{0xEE, 0xF3, 0xFD};
    QColor accentSoftHover{0xE1, 0xEA, 0xFB};
    QColor accentSoftBorder{0xCF, 0xDD, 0xFB};
    QColor accentInk{0x1F, 0x46, 0xB0};
    QColor accentTrack{0xCB, 0xDA, 0xFA};

    // Clinical and device state
    QColor ok{0x12, 0x80, 0x5C};
    QColor okSoft{0xE7, 0xF6, 0xEF};
    QColor okDot{0x17, 0xA6, 0x73};
    QColor warn{0x8A, 0x4A, 0x05};
    QColor warnSoft{0xFE, 0xF4, 0xE6};
    QColor warnDot{0xF0, 0xA2, 0x3B};
    QColor bad{0xC4, 0x32, 0x1F};
    QColor badSoft{0xFD, 0xEC, 0xEA};
    QColor badBorder{0xF0, 0xB4, 0xAB};
    QColor badDot{0xE5, 0x53, 0x3F};

    // Waveform channels A-D: trace colour and chip background
    QColor channel[4] = {{0x2B, 0x5B, 0xD7}, {0x0E, 0x93, 0x84}, {0xD0, 0x8A, 0x1E}, {0x7A, 0x55, 0xE8}};
    QColor channelSoft[4] = {{0xEE, 0xF3, 0xFD}, {0xE6, 0xF6, 0xF3}, {0xFD, 0xF3, 0xE3}, {0xF1, 0xED, 0xFE}};
    QColor channelInk[4] = {{0x2B, 0x5B, 0xD7}, {0x0E, 0x83, 0x76}, {0xA8, 0x64, 0x0B}, {0x6A, 0x45, 0xD8}};
};

const Tokens& tokens();

// Font stacks. Chinese text uses Microsoft YaHei UI; numbers use Segoe UI so
// digits are tabular and crisp; debug read-outs use Consolas.
QString uiFamily();
QFont uiFont(int pixelSize, QFont::Weight weight = QFont::Normal);
QFont numberFont(int pixelSize, QFont::Weight weight = QFont::DemiBold);
QFont monoFont(int pixelSize);

// The application-wide base: Fusion style, a palette built from the tokens and
// the UI font. Idempotent, so MainWindow can call it in tests as well.
void installApplicationStyle();

// resources/theme.qss with every @token replaced.
QString styleSheet();

// Re-evaluates property selectors after a dynamic property change.
void repolish(QWidget* widget);

enum class Tone { None, Info, Ok, Warn, Bad, Muted };
// Sets the "tone" property used by theme.qss and repolishes only on change.
void setTone(QWidget* widget, Tone tone);

// Shared QtCharts look: transparent chart background, sunken plot area,
// dashed grid, no axis line, small muted labels.
void styleChart(QChart* chart);
void styleValueAxis(QValueAxis* axis, bool labelsVisible = true);

} // namespace Theme
