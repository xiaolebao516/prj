#include "measurementguidedialog.h"

#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QPushButton>
#include <QSaveFile>
#include <QStackedWidget>
#include <QTextStream>
#include <QVBoxLayout>
#include <QStringList>
#include <cmath>

namespace {

constexpr int kGuideVersion = 1;

class GuideIllustrationWidget : public QWidget
{
public:
    explicit GuideIllustrationWidget(int pageIndex, QWidget* parent = nullptr)
        : QWidget(parent), pageIndex_(pageIndex)
    {
        setMinimumSize(300, 300);

        const QStringList replacementPaths = {
            QStringLiteral(":/images/measurement-guide-prepare.png"),
            QStringLiteral(":/images/measurement-guide-position.png"),
            QStringLiteral(":/images/measurement-guide-tilt.png")
        };
        if (pageIndex_ >= 0 && pageIndex_ < replacementPaths.size()) {
            replacement_.load(replacementPaths.at(pageIndex_));
        }
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.fillRect(rect(), QColor(241, 247, 251));

        if (!replacement_.isNull()) {
            const QPixmap scaled = replacement_.scaled(
                size() - QSize(24, 24), Qt::KeepAspectRatio,
                Qt::SmoothTransformation);
            painter.drawPixmap(QPoint((width() - scaled.width()) / 2,
                                      (height() - scaled.height()) / 2),
                               scaled);
            return;
        }

        painter.save();
        painter.scale(width() / 360.0, height() / 300.0);

        painter.save();
        painter.translate(180, 174);
        painter.rotate(-10);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(242, 199, 165));
        painter.drawRoundedRect(QRectF(-126, -31, 252, 62), 30, 30);
        painter.restore();

        if (pageIndex_ == 0) {
            painter.save();
            painter.translate(180, 174);
            painter.rotate(-10);
            painter.setBrush(QColor(70, 167, 219, 95));
            painter.drawEllipse(QRectF(-45, -23, 96, 46));
            painter.restore();
            drawDrop(painter, QPointF(86, 76), 17);
            drawDrop(painter, QPointF(119, 52), 12);
        }

        painter.save();
        const QPointF probeCenter = pageIndex_ == 1
            ? QPointF(154, 105) : QPointF(190, 105);
        painter.translate(probeCenter);
        if (pageIndex_ == 2) painter.rotate(-15);
        painter.setPen(QPen(QColor(140, 152, 167), 2));
        painter.setBrush(QColor(229, 233, 239));
        painter.drawRoundedRect(QRectF(-31, -58, 62, 116), 15, 15);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(125, 137, 152));
        painter.drawRoundedRect(QRectF(-24, 52, 48, 15), 5, 5);
        painter.restore();

        painter.setPen(QPen(QColor(45, 126, 201), 4,
                            Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.setBrush(QColor(45, 126, 201));
        if (pageIndex_ == 1) {
            drawDoubleArrow(painter, QPointF(94, 255), QPointF(270, 255));
            drawMiniMeter(painter, QRectF(316, 65, 20, 150));
        } else if (pageIndex_ == 2) {
            QPainterPath arc;
            arc.moveTo(105, 102);
            arc.cubicTo(125, 32, 245, 22, 275, 96);
            painter.drawPath(arc);
            drawArrowHead(painter, QPointF(275, 96), QPointF(254, 75));
            drawMiniMeter(painter, QRectF(24, 65, 20, 150));
        }

        painter.restore();
    }

private:
    static void drawDrop(QPainter& painter, const QPointF& center, qreal size)
    {
        QPainterPath drop;
        drop.moveTo(center.x(), center.y() - size);
        drop.cubicTo(center.x() - size, center.y(),
                     center.x() - size * 0.7, center.y() + size,
                     center.x(), center.y() + size);
        drop.cubicTo(center.x() + size * 0.7, center.y() + size,
                     center.x() + size, center.y(),
                     center.x(), center.y() - size);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(54, 160, 220, 150));
        painter.drawPath(drop);
    }

    static void drawArrowHead(QPainter& painter,
                              const QPointF& tip,
                              const QPointF& directionPoint)
    {
        const QLineF direction(directionPoint, tip);
        const qreal angle = direction.angle() * 3.14159265358979323846 / 180.0;
        const qreal length = 13.0;
        QPolygonF head;
        head << tip
             << tip - QPointF(std::cos(angle - 0.55) * length,
                              -std::sin(angle - 0.55) * length)
             << tip - QPointF(std::cos(angle + 0.55) * length,
                              -std::sin(angle + 0.55) * length);
        painter.drawPolygon(head);
    }

    static void drawDoubleArrow(QPainter& painter,
                                const QPointF& left,
                                const QPointF& right)
    {
        painter.drawLine(left, right);
        drawArrowHead(painter, right, left);
        drawArrowHead(painter, left, right);
    }

    static void drawMiniMeter(QPainter& painter, const QRectF& meter)
    {
        painter.save();
        painter.setPen(QPen(QColor(148, 160, 174), 1));
        painter.setBrush(QColor(232, 237, 242));
        painter.drawRoundedRect(meter, 3, 3);
        painter.setPen(QPen(QColor(48, 49, 51), 2));
        painter.drawLine(QPointF(meter.left(), meter.center().y()),
                         QPointF(meter.right(), meter.center().y()));
        painter.restore();
    }

    int pageIndex_ = 0;
    QPixmap replacement_;
};

QLabel* wrappedLabel(const QString& text, const QString& objectName)
{
    QLabel* label = new QLabel(text);
    label->setObjectName(objectName);
    label->setWordWrap(true);
    label->setTextFormat(Qt::RichText);
    label->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    return label;
}

} // namespace

MeasurementGuideDialog::MeasurementGuideDialog(Mode mode, QWidget* parent)
    : QDialog(parent), mode_(mode)
{
    setObjectName(QStringLiteral("measurementGuideDialog"));
    setWindowTitle(QStringLiteral("检测操作教学"));
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    setModal(true);
    resize(900, 620);
    setMinimumSize(760, 560);
    setStyleSheet(QStringLiteral(
        "QDialog#measurementGuideDialog { background: #FFFFFF; }"
        "QLabel#guideTitle { font-size: 22px; font-weight: bold; color: #303133; }"
        "QLabel#guideSubtitle { color: #606266; }"
        "QLabel#guideHeading { font-size: 18px; font-weight: bold; color: #303133; }"
        "QLabel#guideBody { font-size: 14px; color: #303133; line-height: 1.6; }"
        "QLabel#guideEmphasis { background: #ECF5FF; border-left: 4px solid #409EFF;"
        " padding: 10px; color: #303133; }"
        "QPushButton#guideNextButton { background: #409EFF; color: white;"
        " border: none; border-radius: 5px; padding: 8px 18px; }"
        "QPushButton#guideSkipButton { background: transparent; color: #909399;"
        " border: 1px solid #DCDFE6; border-radius: 5px; padding: 8px 16px; }"));

    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(26, 20, 26, 20);
    root->setSpacing(14);

    QLabel* title = new QLabel(QStringLiteral("检测操作教学"));
    title->setObjectName(QStringLiteral("guideTitle"));
    QLabel* subtitle = new QLabel(
        mode_ == Mode::Automatic
            ? QStringLiteral("首次检测前请先了解探头调整顺序。")
            : QStringLiteral("可随时查看操作步骤；关闭教学不会开始检测。"));
    subtitle->setObjectName(QStringLiteral("guideSubtitle"));
    root->addWidget(title);
    root->addWidget(subtitle);

    pages_ = new QStackedWidget;
    pages_->setObjectName(QStringLiteral("measurementGuidePages"));
    pages_->addWidget(createPage(
        0,
        QStringLiteral("1　测量前准备"),
        QStringLiteral(
            "<ol>"
            "<li>在前臂桡骨测试部位涂抹足量耦合剂，使皮肤与探头之间均匀覆盖。</li>"
            "<li>将探头充分贴合测试部位，先调整右侧 D，再调整左侧 G。</li>"
            "<li>最终目标是让两条进度条均稳定在中线，然后保持探头不动。</li>"
            "<li>每轮累计 30 个有效值；姿势和信号稳定时通常约 5 秒，共完成 5 轮。</li>"
            "</ol>"),
        QStringLiteral("每轮结束后，按空格或点击“开始检测”继续下一轮。")));
    pages_->addWidget(createPage(
        1,
        QStringLiteral("2　先调整右侧 D：空间位置"),
        QStringLiteral(
            "<ol>"
            "<li>保持探头与皮肤充分贴合，不要大幅抬起或转动。</li>"
            "<li>沿测试部位小幅移动探头，同时观察右侧 D 进度条。</li>"
            "<li>找到 D 稳定在中线的位置后，保持当前位置。</li>"
            "</ol>"),
        QStringLiteral("动作幅度要小；不使用固定“向左、向右”指令，避免探头旋转后方向相反。")));
    pages_->addWidget(createPage(
        2,
        QStringLiteral("3　再调整左侧 G：左右倾角"),
        QStringLiteral(
            "<ol>"
            "<li>保持右侧 D 稳定在中线，小幅调整探头左右倾角。</li>"
            "<li>观察左侧 G 进度条，使 G 也稳定在中线。</li>"
            "<li>两条均在中线后保持姿势，系统自动累计本轮 30 个有效值。</li>"
            "<li>完成一轮后按空格或点击“开始检测”，重复至 5 轮完成。</li>"
            "</ol>"),
        QStringLiteral("最终判断始终以右侧 D、左侧 G 两条均稳定在中线为准。")));
    root->addWidget(pages_, 1);

    QHBoxLayout* footer = new QHBoxLayout;
    pageIndicator_ = new QLabel;
    pageIndicator_->setObjectName(QStringLiteral("guidePageIndicator"));
    skipButton_ = new QPushButton;
    skipButton_->setObjectName(QStringLiteral("guideSkipButton"));
    backButton_ = new QPushButton(QStringLiteral("上一步"));
    backButton_->setObjectName(QStringLiteral("guideBackButton"));
    nextButton_ = new QPushButton;
    nextButton_->setObjectName(QStringLiteral("guideNextButton"));
    footer->addWidget(pageIndicator_);
    footer->addStretch();
    footer->addWidget(skipButton_);
    footer->addWidget(backButton_);
    footer->addWidget(nextButton_);
    root->addLayout(footer);

    connect(backButton_, &QPushButton::clicked,
            this, &MeasurementGuideDialog::goBack);
    connect(nextButton_, &QPushButton::clicked,
            this, &MeasurementGuideDialog::goNext);
    connect(skipButton_, &QPushButton::clicked,
            this, &MeasurementGuideDialog::skipOrClose);
    refreshNavigation();
}

int MeasurementGuideDialog::currentGuideVersion()
{
    return kGuideVersion;
}

bool MeasurementGuideDialog::isCurrentVersionSeen(const QString& settingsPath)
{
    QFile file(settingsPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
    const QString content = QString::fromUtf8(file.readAll()).trimmed();
    return content == QStringLiteral("version=%1").arg(kGuideVersion);
}

bool MeasurementGuideDialog::markCurrentVersionSeen(const QString& settingsPath,
                                                      QString* errorMessage)
{
    QSaveFile file(settingsPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        if (errorMessage) *errorMessage = file.errorString();
        return false;
    }
    const QByteArray content =
        QStringLiteral("version=%1\n").arg(kGuideVersion).toUtf8();
    if (file.write(content) != content.size() || !file.commit()) {
        if (errorMessage) *errorMessage = file.errorString();
        return false;
    }
    return true;
}

QWidget* MeasurementGuideDialog::createPage(int pageIndex,
                                             const QString& heading,
                                             const QString& bodyHtml,
                                             const QString& emphasis)
{
    QWidget* page = new QWidget;
    page->setObjectName(QStringLiteral("measurementGuidePage%1").arg(pageIndex + 1));
    QHBoxLayout* layout = new QHBoxLayout(page);
    layout->setContentsMargins(0, 8, 0, 8);
    layout->setSpacing(28);

    GuideIllustrationWidget* illustration =
        new GuideIllustrationWidget(pageIndex, page);
    illustration->setObjectName(
        QStringLiteral("measurementGuideIllustration%1").arg(pageIndex + 1));
    layout->addWidget(illustration, 44);

    QWidget* copy = new QWidget(page);
    QVBoxLayout* copyLayout = new QVBoxLayout(copy);
    copyLayout->setContentsMargins(0, 0, 0, 0);
    copyLayout->setSpacing(12);
    QLabel* headingLabel = wrappedLabel(heading, QStringLiteral("guideHeading"));
    QLabel* bodyLabel = wrappedLabel(bodyHtml, QStringLiteral("guideBody"));
    QLabel* emphasisLabel = wrappedLabel(emphasis, QStringLiteral("guideEmphasis"));
    copyLayout->addWidget(headingLabel);
    copyLayout->addWidget(bodyLabel, 1);
    copyLayout->addWidget(emphasisLabel);
    layout->addWidget(copy, 56);
    return page;
}

void MeasurementGuideDialog::goBack()
{
    if (pages_->currentIndex() > 0) {
        pages_->setCurrentIndex(pages_->currentIndex() - 1);
        refreshNavigation();
    }
}

void MeasurementGuideDialog::goNext()
{
    if (pages_->currentIndex() < pages_->count() - 1) {
        pages_->setCurrentIndex(pages_->currentIndex() + 1);
        refreshNavigation();
        return;
    }
    accept();
}

void MeasurementGuideDialog::skipOrClose()
{
    if (mode_ == Mode::Automatic) accept();
    else reject();
}

void MeasurementGuideDialog::refreshNavigation()
{
    const int page = pages_->currentIndex();
    const int count = pages_->count();
    const QString active = QStringLiteral("<font color='#409EFF'>●</font>");
    const QString inactive = QStringLiteral("<font color='#C0C4CC'>○</font>");
    QString dots;
    for (int index = 0; index < count; ++index) {
        if (!dots.isEmpty()) dots += QStringLiteral("　");
        dots += index == page ? active : inactive;
    }
    pageIndicator_->setText(
        QStringLiteral("%1　%2 / %3").arg(dots).arg(page + 1).arg(count));
    backButton_->setEnabled(page > 0);
    skipButton_->setText(mode_ == Mode::Automatic
                             ? QStringLiteral("跳过教程")
                             : QStringLiteral("关闭"));
    if (page == count - 1) {
        nextButton_->setText(mode_ == Mode::Automatic
                                 ? QStringLiteral("知道了，开始检测")
                                 : QStringLiteral("知道了"));
    } else {
        nextButton_->setText(QStringLiteral("下一步"));
    }
}
