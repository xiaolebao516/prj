#include "measurementguidedialog.h"

#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QProgressBar>
#include <QPushButton>
#include <QSaveFile>
#include <QStackedWidget>
#include <QTextStream>
#include <QVBoxLayout>
#include <QStringList>
#include <cmath>

namespace {

constexpr int kGuideVersion = 2;

class GuideIllustrationWidget : public QWidget
{
public:
    explicit GuideIllustrationWidget(int pageIndex, QWidget* parent = nullptr)
        : QWidget(parent), pageIndex_(pageIndex)
    {
        setMinimumSize(280, 220);
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.fillRect(rect(), QColor("#f2f5f8"));
        // Same shapes and view directions as the approved preview; keep aspect ratio.
        const qreal scale = qMin(width() / 410.0, height() / 238.0);
        p.translate((width() - 410 * scale) / 2, (height() - 238 * scale) / 2);
        p.scale(scale, scale);
        const QColor blue("#147bbf");
        QPainterPath arm;
        if (pageIndex_ < 2) {
            arm.moveTo(35,116); arm.quadTo(25,134,36,163);
            arm.quadTo(47,181,71,178); arm.lineTo(338,164);
            arm.quadTo(369,161,376,145); arm.quadTo(378,125,351,121);
            arm.lineTo(75,101); arm.quadTo(45,98,35,116);
        } else {
            arm.moveTo(60,194); arm.quadTo(82,137,205,137);
            arm.quadTo(328,137,350,194); arm.lineTo(350,220); arm.lineTo(60,220);
        }
        arm.closeSubpath();
        p.setPen(QPen(QColor("#c59d7b"), 1.5));
        p.setBrush(QColor("#efd0b7")); p.drawPath(arm);
        auto probe = [&p]() {
            p.setPen(QPen(QColor("#879bad"), 2)); p.setBrush(QColor("#f8fafc"));
            p.drawRoundedRect(QRectF(-77,-25,154,50),18,18);
            p.setPen(Qt::NoPen); p.setBrush(QColor("#879bad"));
            p.drawRoundedRect(QRectF(-52,-16,13,32),5,5);
            p.drawRoundedRect(QRectF(39,-16,13,32),5,5);
            p.setPen(QPen(QColor("#147bbf"),3)); p.drawLine(-26,0,26,0);
        };
        if (pageIndex_ == 0) {
            p.setPen(Qt::NoPen); p.setBrush(QColor(20,123,191,56));
            p.drawEllipse(QRectF(132,105,150,34));
            p.save(); p.translate(207,91); probe(); p.restore();
            QPainterPath drop;
            drop.moveTo(106,39); drop.cubicTo(99,53,94,57,94,64);
            drop.cubicTo(94,80,118,80,118,64); drop.cubicTo(118,57,112,49,106,39);
            p.setPen(Qt::NoPen); p.setBrush(QColor(20,123,191,150)); p.drawPath(drop);
            p.setPen(QPen(blue,3)); p.drawLine(147,205,267,205);
            p.drawLine(147,205,157,199); p.drawLine(147,205,157,211);
            p.drawLine(267,205,257,199); p.drawLine(267,205,257,211);
        } else if (pageIndex_ == 1) {
            p.setPen(QPen(QColor("#879bad"),2,Qt::DashLine)); p.drawLine(53,142,362,142);
            p.save(); p.translate(205,142); p.rotate(-28); p.setOpacity(.28); probe(); p.restore();
            p.save(); p.translate(205,142); probe(); p.restore();
            p.setPen(QPen(blue,3)); p.setBrush(Qt::NoBrush);
            QPainterPath rotation; rotation.moveTo(154,55); rotation.quadTo(219,27,278,75);
            p.drawPath(rotation); p.drawLine(278,75,263,73); p.drawLine(278,75,275,60);
            p.setPen(QPen(QColor("#879bad"),2,Qt::DashLine)); p.drawLine(101,207,312,207);
        } else {
            p.setPen(QPen(QColor(20,123,191,76),9)); p.setBrush(Qt::NoBrush);
            QPainterPath gel; gel.moveTo(166,141); gel.quadTo(205,132,244,141); p.drawPath(gel);
            p.setPen(QPen(QColor("#879bad"),2)); p.setBrush(QColor("#f8fafc"));
            p.save(); p.translate(205,142); p.rotate(-14); p.setOpacity(.3);
            p.drawRoundedRect(QRectF(-35,-91,70,88),12,12); p.restore();
            p.drawRoundedRect(QRectF(170,51,70,88),12,12);
            p.setPen(Qt::NoPen); p.setBrush(QColor("#879bad"));
            p.drawRoundedRect(QRectF(175,132,60,11),4,4);
            p.setPen(QPen(blue,3)); p.setBrush(Qt::NoBrush);
            QPainterPath tilt; tilt.moveTo(137,76); tilt.quadTo(129,100,140,119);
            tilt.moveTo(275,76); tilt.quadTo(283,100,272,119); p.drawPath(tilt);
            p.drawLine(137,76,127,84); p.drawLine(137,76,142,88);
            p.drawLine(275,76,270,88); p.drawLine(275,76,285,84);
        }
    }

private:
    int pageIndex_ = 0;
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

MeasurementGuideDialog::MeasurementGuideDialog(Mode mode, QWidget* parent, double corrAThreshold)
    : QDialog(parent), mode_(mode), corrAThreshold_(corrAThreshold)
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
        "QLabel#guideEmphasis { background: #ECF5FF; border-left: 4px solid #147bbf;"
        " padding: 10px; color: #303133; }"
        "QPushButton#guideNextButton { background: #147bbf; color: white;"
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

    auto* steps = new QHBoxLayout;
    const QStringList names = {QStringLiteral("① 准备与贴合"), QStringLiteral("② 长轴方向 · corrA"),
                               QStringLiteral("③ 倾角与计数")};
    for (int index = 0; index < names.size(); ++index) {
        stepButtons_[index] = new QPushButton(names[index]);
        stepButtons_[index]->setObjectName(QStringLiteral("guideStep%1").arg(index + 1));
        steps->addWidget(stepButtons_[index], 1);
        connect(stepButtons_[index], &QPushButton::clicked, this, [this, index]() {
            pages_->setCurrentIndex(index);
            refreshNavigation();
        });
    }
    root->addLayout(steps);
    pages_ = new QStackedWidget;
    pages_->setObjectName(QStringLiteral("measurementGuidePages"));
    pages_->addWidget(createPage(
        0, QStringLiteral("放到位，贴合好"),
        QStringLiteral("<p>在前臂桡骨测试部位涂抹足量耦合剂，使皮肤与探头之间充分、均匀覆盖。</p>"
                       "<p>探头放到对应位置，长轴大致沿桡骨方向，保持充分贴合，避免明显倾斜或翘起。</p>"),
        QStringLiteral("<b>接下来主要看 corrA</b><br>不用一开始同时追着每个进度条调整。")));
    pages_->addWidget(createPage(
        1, QStringLiteral("先调整长轴方向，让 corrA 达标"),
        QStringLiteral("<p>保持充分贴合，在皮肤表面小幅旋转探头，使探头长轴大致沿桡骨方向，同时观察 corrA。</p>"
                       "<p>要求 ≥ %1；达到要求即可，不必追求满格。</p>"
                       "<p>corrA 是信号相关性，不是角度，也不是准确率。</p>")
            .arg(corrAThreshold_, 0, 'f', 2),
        QStringLiteral("<b>已经开始连续计数？</b><br>请保持姿势，不必再刻意调整 G。")));
    pages_->addWidget(createPage(
        2, QStringLiteral("仍未计数，再小幅调整倾角"),
        QStringLiteral("<p>corrA 已达标但仍未计数时，尽量保持已找到的位置与长轴方向，小幅改变倾角，参考 G 辅助调整。</p>"
                       "<p>若仍不能推进，再查看 D 是否满足要求，并检查探头贴合。不要持续盲目调整同一个方向。</p>"
                       "<p>每轮累计 30 个有效值，共 5 轮。每轮完成后等待 1 秒自动进入下一轮，也可按空格或点击按钮立即继续。</p>"),
        QStringLiteral("<b>连续计数后，保持探头不动</b><br>提示仅供参考，以有效值计数为准。")));
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
    footer->addWidget(backButton_);
    footer->addWidget(pageIndicator_);
    footer->addStretch();
    footer->addWidget(skipButton_);
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
    auto* figure = new QVBoxLayout;
    const QStringList views = {QStringLiteral("充分贴合 · 均匀涂抹耦合剂"),
                               QStringLiteral("俯视图 · 在皮肤表面小幅旋转"),
                               QStringLiteral("沿探头长轴看 · 仅示意倾角微调")};
    figure->addWidget(illustration, 1);
    figure->addWidget(wrappedLabel(views[pageIndex], QStringLiteral("guideViewLabel")));
    layout->addLayout(figure, 44);

    QWidget* copy = new QWidget(page);
    QVBoxLayout* copyLayout = new QVBoxLayout(copy);
    copyLayout->setContentsMargins(0, 0, 0, 0);
    copyLayout->setSpacing(12);
    QLabel* headingLabel = wrappedLabel(heading, QStringLiteral("guideHeading"));
    QLabel* bodyLabel = wrappedLabel(bodyHtml, QStringLiteral("guideBody"));
    QLabel* emphasisLabel = wrappedLabel(emphasis, QStringLiteral("guideEmphasis"));
    copyLayout->addWidget(headingLabel);
    copyLayout->addWidget(bodyLabel);
    if (pageIndex == 1) {
        auto* example = new QProgressBar;
        example->setObjectName(QStringLiteral("guideCorrAExample"));
        example->setRange(0, 1000);
        example->setValue(846);
        example->setFormat(QStringLiteral("示意 corrA 0.846"));
        example->setStyleSheet(QStringLiteral(
            "QProgressBar {border:1px solid #dbe4ed; border-radius:5px; background:#e8edf3;"
            " text-align:center; min-height:24px;} QProgressBar::chunk {background:#a8d9cc;}"));
        copyLayout->addWidget(example);
    }
    copyLayout->addStretch();
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
    for (int index = 0; index < count; ++index) {
        stepButtons_[index]->setStyleSheet(index == page
            ? QStringLiteral("background:#e9f4fc; color:#147bbf; border:1px solid #147bbf; border-radius:6px; padding:9px;")
            : QStringLiteral("background:#f2f5f8; color:#5e7083; border:1px solid transparent; border-radius:6px; padding:9px;"));
    }
    const QString active = QStringLiteral("<font color='#147bbf'>●</font>");
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
                                 : QStringLiteral("知道了，返回检测"));
    } else {
        nextButton_->setText(QStringLiteral("下一步"));
    }
}
