#include "calibrationdialog.h"

#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QDateTime>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QScrollArea>
#include <QStackedWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <cmath>

namespace {

QLabel* pageTitle(const QString& text)
{
    QLabel* label = new QLabel(text);
    label->setStyleSheet(QStringLiteral("font-size: 18px; font-weight: bold; color: #303133;"));
    return label;
}

QLabel* wrappedLabel(const QString& text)
{
    QLabel* label = new QLabel(text);
    label->setWordWrap(true);
    label->setTextFormat(Qt::RichText);
    label->setStyleSheet(QStringLiteral("line-height: 1.45; color: #606266;"));
    return label;
}

QTableWidget* measurementTable()
{
    QTableWidget* table = new QTableWidget(6, 7);
    table->setHorizontalHeaderLabels({QStringLiteral("次数"),
                                      QStringLiteral("温度(C)"),
                                      QStringLiteral("有效帧"),
                                      QStringLiteral("B通道SOS"),
                                      QStringLiteral("A通道诊断"),
                                      QStringLiteral("B相关"),
                                      QStringLiteral("拒绝帧")});
    table->verticalHeader()->setVisible(false);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setMinimumHeight(235);
    for (int row = 0; row < 6; ++row) {
        table->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1)));
        for (int column = 1; column < 7; ++column) {
            table->setItem(row, column, new QTableWidgetItem(QStringLiteral("--")));
        }
    }
    return table;
}

QString dText(double value)
{
    return QStringLiteral("%1 mm").arg(value * 1000.0, 0, 'f', 4);
}

QString statusText(const QString& status)
{
    if (status == QStringLiteral("ACTIVATED")) return QStringLiteral("已激活");
    if (status == QStringLiteral("FAILED")) return QStringLiteral("工程验证未通过");
    if (status == QStringLiteral("DEVELOPMENT_COMPLETE")) return QStringLiteral("研发演示完成");
    if (status == QStringLiteral("PASSED_NOT_ACTIVATED")) return QStringLiteral("工程验证通过，未激活");
    if (status == QStringLiteral("RESTORED_DEFAULT")) return QStringLiteral("恢复默认参数");
    if (status == QStringLiteral("RESTORED_PREVIOUS")) return QStringLiteral("恢复上一参数");
    return status;
}

} // namespace

CalibrationDialog::CalibrationDialog(CalibrationStore* store,
                                     const QString& operatorName,
                                     QWidget* parent)
    : QDialog(parent), store_(store)
{
    setWindowTitle(QStringLiteral("探头校准"));
    setModal(true);
    resize(980, 760);
    setMinimumSize(820, 650);

    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 16, 20, 16);
    root->setSpacing(12);

    stepLabel_ = new QLabel;
    stepLabel_->setObjectName(QStringLiteral("calibrationStepLabel"));
    stepLabel_->setStyleSheet(QStringLiteral("color: #409EFF; font-weight: bold;"));
    root->addWidget(stepLabel_);

    pages_ = new QStackedWidget;
    pages_->setObjectName(QStringLiteral("calibrationPages"));
    pages_->addWidget(createIntroductionPage());
    pages_->addWidget(createSetupPage());
    pages_->addWidget(createCollectionPage(CalibrationPhase::Calibration));
    pages_->addWidget(createRepositionPage());
    pages_->addWidget(createCollectionPage(CalibrationPhase::Validation));
    pages_->addWidget(createResultPage());
    root->addWidget(pages_, 1);

    QHBoxLayout* footer = new QHBoxLayout;
    backButton_ = new QPushButton(QStringLiteral("上一步"));
    nextButton_ = new QPushButton(QStringLiteral("下一步"));
    closeButton_ = new QPushButton(QStringLiteral("关闭"));
    backButton_->setObjectName(QStringLiteral("calibrationBackButton"));
    nextButton_->setObjectName(QStringLiteral("calibrationNextButton"));
    closeButton_->setObjectName(QStringLiteral("calibrationCloseButton"));
    footer->addWidget(backButton_);
    footer->addStretch();
    footer->addWidget(nextButton_);
    footer->addWidget(closeButton_);
    root->addLayout(footer);

    operatorEdit_->setText(operatorName);
    referenceTemperatureSpin_->setValue(23.0);
    calibrationTemperatureSpin_->setValue(23.0);
    validationTemperatureSpin_->setValue(23.0);
    referenceSosSpin_->setValue(2500.0);

    connect(backButton_, &QPushButton::clicked, this, &CalibrationDialog::goBack);
    connect(nextButton_, &QPushButton::clicked, this, &CalibrationDialog::goNext);
    connect(closeButton_, &QPushButton::clicked, this, &CalibrationDialog::close);
    connect(modeCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CalibrationDialog::updateModeFields);

    refreshCurrentParameters();
    updateModeFields();
    refreshNavigation();
}

void CalibrationDialog::submitFrame(const CalibrationFrame& frame)
{
    if (!session_.isCollecting()) return;
    const CalibrationPhase phase = session_.currentPhase();
    QString reason;
    const CalibrationFrameResult result = session_.addFrame(frame, &reason);

    QProgressBar* progress = progressForPhase(phase);
    QLabel* status = statusForPhase(phase);
    progress->setValue(session_.currentValidFrameCount());
    progress->setFormat(QStringLiteral("有效帧：%v / %m"));

    if (result == CalibrationFrameResult::Rejected) {
        status->setText(QStringLiteral("本帧未计入：%1；累计拒绝 %2 帧。")
                            .arg(reason)
                            .arg(session_.currentRejectedFrameCount()));
        status->setStyleSheet(QStringLiteral("color: #E6A23C;"));
        return;
    }
    if (result == CalibrationFrameResult::Accepted) {
        status->setText(QStringLiteral("正在采集第 %1/6 次独立测量。")
                            .arg(session_.completedMeasurementCount(phase) + 1));
        status->setStyleSheet(QStringLiteral("color: #409EFF;"));
        return;
    }

    emit acquisitionStopRequested();
    finishAcquisitionUi(phase);
}

void CalibrationDialog::notifyAcquisitionUnavailable(const QString& reason)
{
    if (session_.isCollecting()) session_.cancelCurrentMeasurement();
    const CalibrationPhase phase = pages_->currentIndex() == 4
        ? CalibrationPhase::Validation
        : CalibrationPhase::Calibration;
    startButtonForPhase(phase)->setEnabled(true);
    operationCheckForPhase(phase)->setEnabled(true);
    temperatureForPhase(phase)->setEnabled(true);
    statusForPhase(phase)->setText(reason);
    statusForPhase(phase)->setStyleSheet(QStringLiteral("color: #F56C6C;"));
}

void CalibrationDialog::closeEvent(QCloseEvent* event)
{
    if (session_.isCollecting()) {
        emit acquisitionStopRequested();
        session_.cancelCurrentMeasurement();
    }
    if (session_.validationComplete() && !recordSaved_) {
        saveCurrentRecord(false);
    }
    QDialog::closeEvent(event);
}

void CalibrationDialog::goBack()
{
    const int page = pages_->currentIndex();
    if (page <= 0 || session_.isCollecting()) return;
    if (page == 2 && session_.completedMeasurementCount(CalibrationPhase::Calibration) > 0) return;
    pages_->setCurrentIndex(page - 1);
    refreshNavigation();
}

void CalibrationDialog::goNext()
{
    const int page = pages_->currentIndex();
    if (page == 0) {
        pages_->setCurrentIndex(1);
    } else if (page == 1) {
        configureSessionAndContinue();
        return;
    } else if (page == 2 && session_.calibrationComplete()) {
        pages_->setCurrentIndex(3);
    } else if (page == 3) {
        if (!repositionConfirmedCheck_->isChecked()) {
            QMessageBox::information(this,
                                     QStringLiteral("请确认重新放置"),
                                     QStringLiteral("请完成清洁、重新涂耦合剂和重新定位后再继续。"));
            return;
        }
        pages_->setCurrentIndex(4);
    } else if (page == 4 && session_.validationComplete()) {
        pages_->setCurrentIndex(5);
        refreshResult();
    } else if (page == 5) {
        close();
        return;
    }
    refreshNavigation();
}

void CalibrationDialog::startCalibrationMeasurement()
{
    startMeasurement(CalibrationPhase::Calibration);
}

void CalibrationDialog::startValidationMeasurement()
{
    startMeasurement(CalibrationPhase::Validation);
}

void CalibrationDialog::activateCandidate()
{
    if (!session_.canActivate()) return;
    if (QMessageBox::question(
            this,
            QStringLiteral("确认激活候选D"),
            QStringLiteral("激活后，患者测量将使用候选D %1。\n是否确认激活？")
                .arg(dText(session_.candidateD()))) != QMessageBox::Yes) {
        return;
    }
    if (!saveCurrentRecord(true)) return;
    activated_ = true;
    emit activeDChanged(store_->parameters().activeD);
    refreshCurrentParameters();
    refreshResult();
}

void CalibrationDialog::restoreDefault()
{
    if (QMessageBox::question(this,
                              QStringLiteral("恢复默认参数"),
                              QStringLiteral("确认恢复只读默认D 7.8400 mm？该操作会写入参数历史。"))
        != QMessageBox::Yes) {
        return;
    }
    QString error;
    if (!store_->restoreDefault(operatorEdit_->text(), &error)) {
        QMessageBox::warning(this, QStringLiteral("恢复失败"), error);
        return;
    }
    emit activeDChanged(store_->parameters().activeD);
    refreshCurrentParameters();
}

void CalibrationDialog::restorePrevious()
{
    if (QMessageBox::question(this,
                              QStringLiteral("恢复上一参数"),
                              QStringLiteral("确认恢复上一D %1？该操作会写入参数历史。")
                                  .arg(dText(store_->parameters().previousD)))
        != QMessageBox::Yes) {
        return;
    }
    QString error;
    if (!store_->restorePrevious(operatorEdit_->text(), &error)) {
        QMessageBox::warning(this, QStringLiteral("恢复失败"), error);
        return;
    }
    emit activeDChanged(store_->parameters().activeD);
    refreshCurrentParameters();
}

void CalibrationDialog::showHistory()
{
    QDialog dialog(this);
    dialog.setWindowTitle(QStringLiteral("校准历史"));
    dialog.resize(920, 460);
    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    QTableWidget* table = new QTableWidget(store_->history().size(), 9);
    table->setHorizontalHeaderLabels({QStringLiteral("完成时间"),
                                      QStringLiteral("操作者"),
                                      QStringLiteral("探头"),
                                      QStringLiteral("试块"),
                                      QStringLiteral("证书编号"),
                                      QStringLiteral("原D(mm)"),
                                      QStringLiteral("候选D(mm)"),
                                      QStringLiteral("误差/CV"),
                                      QStringLiteral("结果")});
    table->verticalHeader()->setVisible(false);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    table->horizontalHeader()->setStretchLastSection(true);

    for (int row = 0; row < store_->history().size(); ++row) {
        const CalibrationRecord& record = store_->history().at(store_->history().size() - 1 - row);
        const QString values[] = {
            record.finishedAt,
            record.setup.operatorName,
            record.setup.probeId,
            record.setup.phantomModel + QStringLiteral(" ") + record.setup.phantomSerial,
            record.setup.certificateId,
            QString::number(record.oldD * 1000.0, 'f', 4),
            record.candidateD > 0.0 ? QString::number(record.candidateD * 1000.0, 'f', 4) : QStringLiteral("--"),
            record.evaluation.complete
                ? QStringLiteral("%1% / %2%")
                      .arg(record.evaluation.relativeErrorPercent, 0, 'f', 2)
                      .arg(record.evaluation.repeatabilityCvPercent, 0, 'f', 2)
                : QStringLiteral("--"),
            statusText(record.status)
        };
        for (int column = 0; column < 9; ++column) {
            table->setItem(row, column, new QTableWidgetItem(values[column].trimmed()));
        }
    }
    layout->addWidget(table);
    QPushButton* close = new QPushButton(QStringLiteral("关闭"));
    connect(close, &QPushButton::clicked, &dialog, &QDialog::accept);
    layout->addWidget(close, 0, Qt::AlignRight);
    dialog.exec();
}

void CalibrationDialog::updateModeFields()
{
    const bool formal = modeCombo_->currentData().toString() == QStringLiteral("formal");
    phantomModelEdit_->setEnabled(formal);
    phantomSerialEdit_->setEnabled(formal);
    certificateEdit_->setEnabled(formal);
    certificateConfirmedCheck_->setEnabled(formal);
    setupModeHint_->setText(formal
        ? QStringLiteral("正式模式会记录可追溯资料，工程验证通过后才允许激活候选D。软件不验证证书真实性。")
        : QStringLiteral("研发演示模式可以验证公式和流程，但无论结果如何都不能激活候选D。"));
}

QWidget* CalibrationDialog::createIntroductionPage()
{
    QWidget* page = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->addWidget(pageTitle(QStringLiteral("探头有效基线D校准")));
    layout->addWidget(wrappedLabel(
        QStringLiteral("本流程根据标准试块在指定温度下的参考SOS，对探头C-D有效基线D生成候选值。"
                       "候选值必须使用重新耦合后的独立数据验证，通过后仍需操作者确认才会生效。")));

    QGroupBox* parameters = new QGroupBox(QStringLiteral("当前参数"));
    QGridLayout* grid = new QGridLayout(parameters);
    currentDLabel_ = new QLabel;
    previousDLabel_ = new QLabel;
    defaultDLabel_ = new QLabel;
    grid->addWidget(new QLabel(QStringLiteral("当前D：")), 0, 0);
    grid->addWidget(currentDLabel_, 0, 1);
    grid->addWidget(new QLabel(QStringLiteral("上一D：")), 1, 0);
    grid->addWidget(previousDLabel_, 1, 1);
    grid->addWidget(new QLabel(QStringLiteral("只读默认D：")), 2, 0);
    grid->addWidget(defaultDLabel_, 2, 1);
    QPushButton* history = new QPushButton(QStringLiteral("查看校准历史"));
    QPushButton* previous = new QPushButton(QStringLiteral("恢复上一参数"));
    QPushButton* defaults = new QPushButton(QStringLiteral("恢复默认参数"));
    grid->addWidget(history, 0, 2);
    grid->addWidget(previous, 1, 2);
    grid->addWidget(defaults, 2, 2);
    connect(history, &QPushButton::clicked, this, &CalibrationDialog::showHistory);
    connect(previous, &QPushButton::clicked, this, &CalibrationDialog::restorePrevious);
    connect(defaults, &QPushButton::clicked, this, &CalibrationDialog::restoreDefault);
    layout->addWidget(parameters);

    QGroupBox* boundary = new QGroupBox(QStringLiteral("结果说明"));
    QVBoxLayout* boundaryLayout = new QVBoxLayout(boundary);
    boundaryLayout->addWidget(wrappedLabel(
        QStringLiteral("页面采用的工程验证阈值为：B通道平均SOS相对误差不超过 <b>±2%</b>，"
                       "6次验证结果的变异系数不超过 <b>1%</b>。数值参考YY/T 0774-2019产品性能要求，"
                       "不等同于标准规定了内部D调整方法，也不代表计量校准合格。")));
    layout->addWidget(boundary);
    layout->addStretch();
    return page;
}

QWidget* CalibrationDialog::createSetupPage()
{
    QWidget* page = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->addWidget(pageTitle(QStringLiteral("填写试块和参考资料")));
    layout->addWidget(wrappedLabel(
        QStringLiteral("请核对证书上的试块型号和序列号，并通过签发机构官网、二维码或联系方式核验证书。"
                       "从证书温度-SOS表中选择与当前温度对应的参考点；软件只记录输入内容，不能验证证书真实性。")));

    QFormLayout* form = new QFormLayout;
    modeCombo_ = new QComboBox;
    modeCombo_->setObjectName(QStringLiteral("calibrationModeCombo"));
    modeCombo_->addItem(QStringLiteral("正式校准"), QStringLiteral("formal"));
    modeCombo_->addItem(QStringLiteral("研发演示（不可激活）"), QStringLiteral("development"));
    operatorEdit_ = new QLineEdit;
    operatorEdit_->setObjectName(QStringLiteral("calibrationOperatorEdit"));
    operatorEdit_->setReadOnly(true);
    probeIdEdit_ = new QLineEdit;
    phantomModelEdit_ = new QLineEdit;
    phantomSerialEdit_ = new QLineEdit;
    certificateEdit_ = new QLineEdit;
    probeIdEdit_->setObjectName(QStringLiteral("calibrationProbeIdEdit"));
    phantomModelEdit_->setObjectName(QStringLiteral("calibrationPhantomModelEdit"));
    phantomSerialEdit_->setObjectName(QStringLiteral("calibrationPhantomSerialEdit"));
    certificateEdit_->setObjectName(QStringLiteral("calibrationCertificateEdit"));
    referenceTemperatureSpin_ = new QDoubleSpinBox;
    referenceTemperatureSpin_->setObjectName(QStringLiteral("calibrationReferenceTemperature"));
    referenceTemperatureSpin_->setRange(-20.0, 80.0);
    referenceTemperatureSpin_->setDecimals(1);
    referenceTemperatureSpin_->setSuffix(QStringLiteral(" C"));
    referenceSosSpin_ = new QDoubleSpinBox;
    referenceSosSpin_->setObjectName(QStringLiteral("calibrationReferenceSos"));
    referenceSosSpin_->setRange(500.0, 10000.0);
    referenceSosSpin_->setDecimals(1);
    referenceSosSpin_->setSuffix(QStringLiteral(" m/s"));
    form->addRow(QStringLiteral("模式"), modeCombo_);
    form->addRow(QStringLiteral("操作者"), operatorEdit_);
    form->addRow(QStringLiteral("探头编号"), probeIdEdit_);
    form->addRow(QStringLiteral("试块型号"), phantomModelEdit_);
    form->addRow(QStringLiteral("试块序列号"), phantomSerialEdit_);
    form->addRow(QStringLiteral("证书编号"), certificateEdit_);
    form->addRow(QStringLiteral("证书参考温度"), referenceTemperatureSpin_);
    form->addRow(QStringLiteral("该温度对应的标准SOS"), referenceSosSpin_);
    layout->addLayout(form);

    setupModeHint_ = wrappedLabel(QString());
    setupModeHint_->setStyleSheet(QStringLiteral("color: #409EFF;"));
    layout->addWidget(setupModeHint_);
    certificateConfirmedCheck_ = new QCheckBox(
        QStringLiteral("我已核对试块与证书，并确认参考温度、SOS和单位录入正确"));
    responsibilityCheck_ = new QCheckBox(
        QStringLiteral("我理解软件不能验证证书、人工温度和实际耦合操作的真实性"));
    certificateConfirmedCheck_->setObjectName(QStringLiteral("calibrationCertificateConfirmed"));
    responsibilityCheck_->setObjectName(QStringLiteral("calibrationResponsibilityConfirmed"));
    layout->addWidget(certificateConfirmedCheck_);
    layout->addWidget(responsibilityCheck_);
    layout->addStretch();
    return page;
}

QWidget* CalibrationDialog::createCollectionPage(CalibrationPhase phase)
{
    const bool calibration = phase == CalibrationPhase::Calibration;
    QWidget* page = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->addWidget(pageTitle(calibration
                                    ? QStringLiteral("校准采集：6次独立测量")
                                    : QStringLiteral("独立验证：重新耦合后6次测量")));
    layout->addWidget(wrappedLabel(
        calibration
            ? QStringLiteral("每次测量固定探头并累计30个有效帧。每次完成后取下探头，下一次重新清洁、涂胶和定位。")
            : QStringLiteral("候选D只在本验证页面的临时处理器中使用。患者测量仍保持原D，直到工程验证通过并确认激活。")));

    QLabel* operationGuide = wrappedLabel(
        QStringLiteral("操作要求：使用同一种耦合剂，薄而均匀地覆盖接触面，排除气泡；保持探头方向、位置和压力一致。"
                       "请使用外部温度计测量试块本体温度，室温不能直接代替试块温度。"));
    operationGuide->setStyleSheet(QStringLiteral("background: #F4F8FF; border: 1px solid #D9ECFF; padding: 10px;"));
    layout->addWidget(operationGuide);

    QHBoxLayout* controls = new QHBoxLayout;
    QDoubleSpinBox* temperature = new QDoubleSpinBox;
    temperature->setRange(-20.0, 80.0);
    temperature->setDecimals(1);
    temperature->setSuffix(QStringLiteral(" C"));
    QCheckBox* operation = new QCheckBox(
        QStringLiteral("已重新清洁、涂胶、排除气泡并确认试块温度"));
    QPushButton* start = new QPushButton(QStringLiteral("开始本次采集"));
    temperature->setObjectName(calibration
                                   ? QStringLiteral("calibrationMeasurementTemperature")
                                   : QStringLiteral("validationMeasurementTemperature"));
    operation->setObjectName(calibration
                                 ? QStringLiteral("calibrationOperationConfirmed")
                                 : QStringLiteral("validationOperationConfirmed"));
    start->setObjectName(calibration
                             ? QStringLiteral("calibrationStartButton")
                             : QStringLiteral("validationStartButton"));
    controls->addWidget(new QLabel(QStringLiteral("当前试块温度：")));
    controls->addWidget(temperature);
    controls->addWidget(operation, 1);
    controls->addWidget(start);
    layout->addLayout(controls);

    QProgressBar* progress = new QProgressBar;
    progress->setObjectName(calibration
                                ? QStringLiteral("calibrationProgress")
                                : QStringLiteral("validationProgress"));
    progress->setRange(0, CalibrationSession::RequiredFramesPerMeasurement);
    progress->setFormat(QStringLiteral("有效帧：%v / %m"));
    layout->addWidget(progress);
    QTableWidget* table = measurementTable();
    table->setObjectName(calibration
                             ? QStringLiteral("calibrationMeasurementTable")
                             : QStringLiteral("validationMeasurementTable"));
    layout->addWidget(table);
    QLabel* status = wrappedLabel(QStringLiteral("等待开始。"));
    layout->addWidget(status);

    if (calibration) {
        calibrationTemperatureSpin_ = temperature;
        calibrationOperationCheck_ = operation;
        calibrationProgress_ = progress;
        calibrationTable_ = table;
        calibrationStatus_ = status;
        calibrationStartButton_ = start;
        candidateLabel_ = new QLabel(QStringLiteral("候选D：完成6次后计算"));
        candidateLabel_->setStyleSheet(QStringLiteral("font-weight: bold; color: #409EFF;"));
        layout->addWidget(candidateLabel_);
        connect(start, &QPushButton::clicked,
                this, &CalibrationDialog::startCalibrationMeasurement);
    } else {
        validationTemperatureSpin_ = temperature;
        validationOperationCheck_ = operation;
        validationProgress_ = progress;
        validationTable_ = table;
        validationStatus_ = status;
        validationStartButton_ = start;
        connect(start, &QPushButton::clicked,
                this, &CalibrationDialog::startValidationMeasurement);
    }
    return page;
}

QWidget* CalibrationDialog::createRepositionPage()
{
    QWidget* page = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->addWidget(pageTitle(QStringLiteral("重新放置后再验证")));
    layout->addWidget(wrappedLabel(
        QStringLiteral("独立验证不能复用前6次的放置状态。请执行以下操作：")));
    layout->addWidget(wrappedLabel(
        QStringLiteral("1. 取下探头并清洁探头与试块接触面。<br>"
                       "2. 使用同一种耦合剂重新均匀涂抹，排除明显气泡。<br>"
                       "3. 按相同方向和压力重新定位；有固定夹具时优先使用。<br>"
                       "4. 使用外部温度计重新测量试块本体温度。<br>"
                       "5. 软件无法检测耦合剂用量、探头压力或温度输入是否真实。")));
    repositionConfirmedCheck_ = new QCheckBox(
        QStringLiteral("以上步骤已经完成，准备使用新的耦合和放置状态进行验证"));
    repositionConfirmedCheck_->setObjectName(QStringLiteral("calibrationRepositionConfirmed"));
    layout->addWidget(repositionConfirmedCheck_);
    layout->addStretch();
    return page;
}

QWidget* CalibrationDialog::createResultPage()
{
    QWidget* page = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->addWidget(pageTitle(QStringLiteral("工程验证结果")));
    resultLabel_ = wrappedLabel(QStringLiteral("等待验证完成。"));
    resultLabel_->setObjectName(QStringLiteral("calibrationResultLabel"));
    resultLabel_->setStyleSheet(QStringLiteral("font-size: 15px;"));
    layout->addWidget(resultLabel_);

    QLabel* threshold = wrappedLabel(
        QStringLiteral("判定阈值：B通道平均SOS相对误差不超过 <b>±2%</b>，6次验证结果变异系数不超过 <b>1%</b>。"
                       "该阈值参考YY/T 0774-2019产品性能要求，用于本项目工程验证，不等同于计量校准结论。"));
    threshold->setStyleSheet(QStringLiteral("background: #FFF8E8; border: 1px solid #F3D19E; padding: 10px;"));
    layout->addWidget(threshold);
    activateButton_ = new QPushButton(QStringLiteral("确认激活候选D"));
    activateButton_->setObjectName(QStringLiteral("calibrationActivateButton"));
    connect(activateButton_, &QPushButton::clicked,
            this, &CalibrationDialog::activateCandidate);
    layout->addWidget(activateButton_, 0, Qt::AlignLeft);
    layout->addStretch();
    return page;
}

void CalibrationDialog::configureSessionAndContinue()
{
    const bool formal = modeCombo_->currentData().toString() == QStringLiteral("formal");
    if (!responsibilityCheck_->isChecked()
        || (formal && !certificateConfirmedCheck_->isChecked())) {
        QMessageBox::information(this,
                                 QStringLiteral("请确认资料"),
                                 QStringLiteral("请完成资料核对和责任边界确认。"));
        return;
    }

    CalibrationSetup setup;
    setup.mode = formal ? CalibrationMode::Formal : CalibrationMode::Development;
    setup.operatorName = operatorEdit_->text();
    setup.probeId = probeIdEdit_->text();
    setup.phantomModel = phantomModelEdit_->text();
    setup.phantomSerial = phantomSerialEdit_->text();
    setup.certificateId = certificateEdit_->text();
    setup.referenceTemperature = referenceTemperatureSpin_->value();
    setup.referenceSos = referenceSosSpin_->value();

    QString error;
    if (!session_.configure(setup, store_->parameters().activeD, &error)) {
        QMessageBox::warning(this, QStringLiteral("资料不完整"), error);
        return;
    }
    calibrationTemperatureSpin_->setValue(setup.referenceTemperature);
    validationTemperatureSpin_->setValue(setup.referenceTemperature);
    pages_->setCurrentIndex(2);
    refreshNavigation();
}

void CalibrationDialog::startMeasurement(CalibrationPhase phase)
{
    if (!operationCheckForPhase(phase)->isChecked()) {
        QMessageBox::information(this,
                                 QStringLiteral("请确认操作"),
                                 QStringLiteral("请先完成清洁、耦合、定位和试块温度测量。"));
        return;
    }
    QString error;
    if (!session_.beginMeasurement(phase, temperatureForPhase(phase)->value(), &error)) {
        QMessageBox::warning(this, QStringLiteral("无法开始采集"), error);
        return;
    }

    progressForPhase(phase)->setValue(0);
    statusForPhase(phase)->setText(
        QStringLiteral("正在采集第 %1/6 次独立测量，请勿移动探头。")
            .arg(session_.completedMeasurementCount(phase) + 1));
    statusForPhase(phase)->setStyleSheet(QStringLiteral("color: #409EFF;"));
    startButtonForPhase(phase)->setEnabled(false);
    operationCheckForPhase(phase)->setEnabled(false);
    temperatureForPhase(phase)->setEnabled(false);
    emit acquisitionStartRequested(session_.processingD());
}

void CalibrationDialog::finishAcquisitionUi(CalibrationPhase phase)
{
    startButtonForPhase(phase)->setEnabled(
        session_.completedMeasurementCount(phase) < CalibrationSession::RequiredMeasurementsPerPhase);
    operationCheckForPhase(phase)->setEnabled(true);
    operationCheckForPhase(phase)->setChecked(false);
    temperatureForPhase(phase)->setEnabled(true);
    progressForPhase(phase)->setValue(CalibrationSession::RequiredFramesPerMeasurement);
    statusForPhase(phase)->setText(
        QStringLiteral("第 %1/6 次独立测量完成。请取下探头，下一次重新清洁、涂胶和定位。")
            .arg(session_.completedMeasurementCount(phase)));
    statusForPhase(phase)->setStyleSheet(QStringLiteral("color: #67C23A;"));
    refreshMeasurementTables();

    if (phase == CalibrationPhase::Calibration && session_.calibrationComplete()) {
        candidateLabel_->setText(QStringLiteral("候选D：%1；验证前患者参数仍为 %2")
                                     .arg(dText(session_.candidateD()),
                                          dText(store_->parameters().activeD)));
    }
    if (phase == CalibrationPhase::Validation && session_.validationComplete()) {
        saveCurrentRecord(false);
        refreshResult();
    }
    refreshNavigation();
}

void CalibrationDialog::refreshCurrentParameters()
{
    currentDLabel_->setText(dText(store_->parameters().activeD));
    previousDLabel_->setText(dText(store_->parameters().previousD));
    defaultDLabel_->setText(dText(CalibrationStore::FactoryDefaultD));
}

void CalibrationDialog::refreshMeasurementTables()
{
    const auto fill = [](QTableWidget* table, const QList<CalibrationMeasurement>& values) {
        for (int row = 0; row < table->rowCount(); ++row) {
            for (int column = 1; column < table->columnCount(); ++column) {
                table->item(row, column)->setText(QStringLiteral("--"));
            }
        }
        for (int row = 0; row < values.size() && row < table->rowCount(); ++row) {
            const CalibrationMeasurement& value = values.at(row);
            table->item(row, 1)->setText(QString::number(value.temperature, 'f', 1));
            table->item(row, 2)->setText(QString::number(value.validFrameCount));
            table->item(row, 3)->setText(QString::number(value.meanSosB, 'f', 1));
            table->item(row, 4)->setText(value.aDiagnosticAvailable
                                             ? QString::number(value.meanSosA, 'f', 1)
                                             : QStringLiteral("不可用"));
            table->item(row, 5)->setText(QString::number(value.meanCorrB, 'f', 3));
            table->item(row, 6)->setText(QString::number(value.rejectedFrameCount));
        }
    };
    fill(calibrationTable_, session_.record().calibrationMeasurements);
    fill(validationTable_, session_.record().validationMeasurements);
}

void CalibrationDialog::refreshNavigation()
{
    const int page = pages_->currentIndex();
    static const QString names[] = {
        QStringLiteral("说明"), QStringLiteral("资料"), QStringLiteral("校准采集"),
        QStringLiteral("重新放置"), QStringLiteral("独立验证"), QStringLiteral("结果")
    };
    stepLabel_->setText(QStringLiteral("步骤 %1/6：%2").arg(page + 1).arg(names[page]));
    backButton_->setEnabled(page > 0
                            && !session_.isCollecting()
                            && !(page == 2
                                 && session_.completedMeasurementCount(CalibrationPhase::Calibration) > 0)
                            && page < 3);
    nextButton_->setEnabled(!session_.isCollecting());
    nextButton_->setText(page == 5 ? QStringLiteral("完成") : QStringLiteral("下一步"));
    if (page == 2) nextButton_->setEnabled(session_.calibrationComplete());
    if (page == 4) nextButton_->setEnabled(session_.validationComplete());
}

void CalibrationDialog::refreshResult()
{
    if (!session_.validationComplete()) return;
    const CalibrationRecord& record = session_.record();
    const CalibrationEvaluation& evaluation = record.evaluation;
    const QString verdict = evaluation.passed
        ? QStringLiteral("工程验证通过")
        : QStringLiteral("工程验证未通过");
    QString details = QStringLiteral(
        "<b>%1</b><br><br>原D：%2<br>候选D：%3<br>"
        "验证B通道平均SOS：%4 m/s<br>相对误差：%5%（限值±2%）<br>"
        "重复性变异系数：%6%（限值1%）")
        .arg(verdict,
             dText(record.oldD),
             dText(record.candidateD),
             QString::number(evaluation.meanSosB, 'f', 1),
             QString::number(evaluation.relativeErrorPercent, 'f', 2),
             QString::number(evaluation.repeatabilityCvPercent, 'f', 2));
    if (!evaluation.passed) {
        details += QStringLiteral("<br>失败原因：%1<br><br>原D保持不变。")
                       .arg(evaluation.failureReason);
    } else if (record.setup.mode == CalibrationMode::Development) {
        details += QStringLiteral("<br><br>当前为研发演示模式，候选D不能激活。原D保持不变。");
    } else if (activated_) {
        details += QStringLiteral("<br><br>候选D已经激活并保存。");
    } else {
        details += QStringLiteral("<br><br>候选D尚未激活，患者测量仍使用原D。");
    }
    resultLabel_->setText(details);
    resultLabel_->setStyleSheet(evaluation.passed
                                    ? QStringLiteral("font-size: 15px; color: #67C23A;")
                                    : QStringLiteral("font-size: 15px; color: #F56C6C;"));
    activateButton_->setVisible(record.setup.mode == CalibrationMode::Formal);
    activateButton_->setEnabled(session_.canActivate() && !activated_);
}

bool CalibrationDialog::saveCurrentRecord(bool activate)
{
    QString error;
    if (!store_->saveCompletedSession(session_.record(), activate, &error)) {
        QMessageBox::warning(this, QStringLiteral("保存校准结果失败"), error);
        return false;
    }
    recordSaved_ = true;
    return true;
}

QTableWidget* CalibrationDialog::tableForPhase(CalibrationPhase phase) const
{
    return phase == CalibrationPhase::Calibration ? calibrationTable_ : validationTable_;
}

QProgressBar* CalibrationDialog::progressForPhase(CalibrationPhase phase) const
{
    return phase == CalibrationPhase::Calibration ? calibrationProgress_ : validationProgress_;
}

QLabel* CalibrationDialog::statusForPhase(CalibrationPhase phase) const
{
    return phase == CalibrationPhase::Calibration ? calibrationStatus_ : validationStatus_;
}

QPushButton* CalibrationDialog::startButtonForPhase(CalibrationPhase phase) const
{
    return phase == CalibrationPhase::Calibration ? calibrationStartButton_ : validationStartButton_;
}

QCheckBox* CalibrationDialog::operationCheckForPhase(CalibrationPhase phase) const
{
    return phase == CalibrationPhase::Calibration ? calibrationOperationCheck_ : validationOperationCheck_;
}

QDoubleSpinBox* CalibrationDialog::temperatureForPhase(CalibrationPhase phase) const
{
    return phase == CalibrationPhase::Calibration ? calibrationTemperatureSpin_ : validationTemperatureSpin_;
}
