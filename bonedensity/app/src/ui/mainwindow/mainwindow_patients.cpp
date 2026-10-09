// 档案与检测记录：加载保存、当前被测者、档案页、历史、导出、备份

#include "mainwindow/mainwindow.h"
#include "ui_mainwindow.h"
#include "mainwindow/mainwindow_internal.h"

#include "dialogs/patientformdialog.h"
#include "storage/databackup.h"

#include <QCheckBox>
#include <QDateEdit>
#include <QDialog>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSaveFile>
#include <QSet>
#include <QSignalBlocker>
#include <QStandardPaths>
#include <QStatusBar>
#include <QTableWidget>
#include <QVBoxLayout>
#include <algorithm>

using namespace mainwindow_detail;

void MainWindow::on_btnPatientInfo_clicked()
{
    if (patientMeasureRunning) return;
    on_btnArchive_clicked();
}

void MainWindow::updateCurrentPatientUI() {
    const QString empty = QStringLiteral("--");
    const auto value = [&empty](const QString& text, const QString& unit = QString()) {
        return text.trimmed().isEmpty() ? empty : text + unit;
    };
    ui->labelName->setText(currentPatient.name.isEmpty() ? QStringLiteral("未选择") : currentPatient.name);
    QStringList meta;
    if (!currentPatient.gender.isEmpty()) meta << currentPatient.gender;
    const int age = ageOnDate(currentPatient.birthDay, QDate::currentDate());
    if (age >= 0) meta << QStringLiteral("%1 岁").arg(age);
    if (lblPatientMeta) lblPatientMeta->setText(meta.join(QStringLiteral(" · ")));
    ui->labelID->setText(QStringLiteral("编号　%1").arg(value(currentPatient.id)));
    ui->labelGender->setText(QStringLiteral("性别　%1").arg(value(currentPatient.gender)));
    ui->labelBirth->setText(QStringLiteral("出生　%1").arg(value(currentPatient.birthDay)));
    ui->labelHeight->setText(QStringLiteral("身高　%1").arg(value(currentPatient.height, QStringLiteral(" cm"))));
    ui->labelWeight->setText(QStringLiteral("体重　%1").arg(value(currentPatient.weight, QStringLiteral(" kg"))));
}

// ==================== 档案管理 ===============================================================================================
void MainWindow::on_btnArchive_clicked() {
    if (patientMeasureRunning) {
        QMessageBox::warning(this, "检测进行中", "检测进行中不能编辑或删除档案。");
        return;
    }
    refreshTable(patientList);
    ui->stackedWidget->setCurrentWidget(ui->pageArchive);
    ui->editSearchKeyword->setFocus(Qt::OtherFocusReason);
}

void MainWindow::on_btnBackFromArchive_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->pageMain);
    scheduleResponsiveLayout();
}

void MainWindow::loadPatients() {
    patientDataWritable = false;
    patientDataLoadError.clear();
    QString errorMessage;
    if (!patientStore.load(xmlFilePath, measurementsFilePath,
                           &patientList, &measurementList, &errorMessage)) {
        patientDataLoadError = errorMessage;
        QMessageBox::warning(
            this,
            "档案加载失败，已进入只读保护",
            errorMessage + "\n\n为避免覆盖原档案，本次运行已禁止新增、修改、删除和检测保存。"
                           "请保留提示中的异常副本，修复数据文件后重新启动软件。");
        updatePatientSelectionUi();
        return;
    }
    patientDataWritable = true;
    updatePatientSelectionUi();
}

QList<MeasurementRecord> MainWindow::measurementsForPatient(const QString& patientId) const
{
    QList<MeasurementRecord> records;
    for (const MeasurementRecord& record : measurementList) {
        if (record.patientId == patientId) records.append(record);
    }
    return records;
}

const MeasurementRecord* MainWindow::latestMeasurementForPatient(const QString& patientId) const
{
    const MeasurementRecord* latest = nullptr;
    for (const MeasurementRecord& record : measurementList) {
        if (record.patientId == patientId &&
            (!latest || record.measuredAt > latest->measuredAt)) {
            latest = &record;
        }
    }
    return latest;
}

bool MainWindow::ensurePatientDataWritable()
{
    if (QFileInfo::exists(xmlFilePath + QStringLiteral(".txn"))) {
        patientDataWritable = false;
        patientDataLoadError = QStringLiteral(
            "检测到未完成的档案保存事务。为避免覆盖可恢复数据，本次运行已进入只读保护。"
            "请关闭并重新启动软件，让程序先完成档案恢复。");
        updatePatientSelectionUi();
    }
    if (patientDataWritable) return true;

    const QString detail = patientDataLoadError.trimmed().isEmpty()
        ? QStringLiteral("档案加载或恢复未成功。")
        : patientDataLoadError;
    QMessageBox::warning(this, "档案处于只读保护",
                         detail + QStringLiteral("\n本次运行禁止新增、修改、删除和检测保存。"));
    return false;
}

bool MainWindow::savePatients(const QList<PatientInfo>& patients)
{
    if (!ensurePatientDataWritable()) return false;
    QString errorMessage;
    if (!patientStore.savePatients(xmlFilePath, patients, &errorMessage)) {
        QMessageBox::warning(this, "档案保存失败", errorMessage);
        return false;
    }
    return true;
}

bool MainWindow::saveMeasurements(const QList<MeasurementRecord>& measurements)
{
    if (!ensurePatientDataWritable()) return false;
    QString errorMessage;
    if (!patientStore.saveMeasurements(measurementsFilePath, measurements, &errorMessage)) {
        QMessageBox::warning(this, "结果保存失败", errorMessage);
        return false;
    }
    return true;
}

bool MainWindow::savePatientData(const QList<PatientInfo>& patients,
                                 const QList<MeasurementRecord>& measurements)
{
    if (!ensurePatientDataWritable()) return false;
    QString errorMessage;
    if (!patientStore.savePatientData(xmlFilePath, measurementsFilePath,
                                      patients, measurements, &errorMessage)) {
        if (QFileInfo::exists(xmlFilePath + QStringLiteral(".txn"))) {
            patientDataWritable = false;
            patientDataLoadError = QStringLiteral(
                "档案保存事务未能完成。为避免覆盖可恢复数据，本次运行已进入只读保护。"
                "请关闭并重新启动软件完成恢复。");
            updatePatientSelectionUi();
            errorMessage += QStringLiteral("\n\n") + patientDataLoadError;
        }
        QMessageBox::critical(this, "档案保存失败", errorMessage);
        return false;
    }
    return true;
}

bool MainWindow::confirmPatientChange(const QString& targetPatientId)
{
    if (!currentPatient.id.isEmpty() && currentPatient.id == targetPatientId) {
        return true;
    }

    if (hasPendingMeasurement) {
        return QMessageBox::question(
                   this,
                   QStringLiteral("放弃未保存结果"),
                   QStringLiteral("当前结果尚未保存，是否放弃并更换被测者？"))
            == QMessageBox::Yes;
    }

    if (hasIncompletePatientRounds()) {
        return QMessageBox::question(
                   this,
                   QStringLiteral("放弃未完成检测"),
                   QStringLiteral("当前被测者已完成 %1/%2 次测量。更换被测者会清空本组进度，"
                                  "是否仍要更换？")
                       .arg(session.roundSos.size())
                       .arg(mCfg.roundsPerMeasurement))
            == QMessageBox::Yes;
    }
    return true;
}

void MainWindow::applyCurrentPatient(const PatientInfo& patient)
{
    currentPatient = patient;
    hasPendingMeasurement = false;
    pendingMeasurement = MeasurementRecord();
    resetAllPatientMeasurementData();
    updateCurrentPatientUI();
    updatePatientSelectionUi();
    updateAgeSosReference();
    updateResultPanel();
}

bool MainWindow::selectCurrentPatient(const PatientInfo& patient)
{
    if (patientMeasureRunning) return false;
    if (!currentPatient.id.isEmpty() && currentPatient.id == patient.id) {
        currentPatient = patient;
        updateCurrentPatientUI();
        updatePatientSelectionUi();
        updateAgeSosReference();
        updateResultPanel();
        return true;
    }
    if (!confirmPatientChange(patient.id)) return false;
    applyCurrentPatient(patient);
    return true;
}

void MainWindow::clearCurrentPatient()
{
    currentPatient = PatientInfo();
    hasPendingMeasurement = false;
    pendingMeasurement = MeasurementRecord();
    resetAllPatientMeasurementData();
    updateCurrentPatientUI();
    updatePatientSelectionUi();
    updateAgeSosReference();
    updateResultPanel();
}

void MainWindow::updateAgeSosReference()
{
    if (!ui || !ui->chartViewReference) return;
    if (!hasCurrentPatient()) {
        ui->chartViewReference->clearReferenceData();
        return;
    }

    const MeasurementRecord* latest = nullptr;
    QDateTime latestTime;
    for (const MeasurementRecord& record : measurementList) {
        if (record.patientId != currentPatient.id) continue;
        const int age = measurementAge(record, currentPatient);
        const QString gender = measurementGender(record, currentPatient);
        bool sosOk = false;
        const double sos = record.sos.trimmed().toDouble(&sosOk);
        if (!sosOk || !AgeSosChartWidget::supportsPoint(gender, age, sos)) continue;
        const QDateTime time = parsedMeasurementDateTime(record.measuredAt);
        if (!latest || (time.isValid() && (!latestTime.isValid() || time > latestTime)) ||
            (!time.isValid() && !latestTime.isValid() && record.measuredAt > latest->measuredAt)) {
            latest = &record;
            latestTime = time;
        }
    }
    if (!latest) {
        const int age = ageOnDate(currentPatient.birthDay, QDate::currentDate());
        AgeSosChartData data;
        data.hasPatient = true;
        data.hasMeasurementRecords = !measurementsForPatient(currentPatient.id).isEmpty();
        data.gender = currentPatient.gender;
        data.focalAge = age;
        ui->chartViewReference->setChartData(data);
        return;
    }
    ui->chartViewReference->setChartData(
        buildAgeSosChartData(currentPatient, *latest, false));
}

void MainWindow::showPatientHistory(const QString& patientId)
{
    QDialog dialog(this);
    dialog.setObjectName(QStringLiteral("historyDialog"));
    PatientInfo historyPatient;
    for (const PatientInfo& patient : patientList) {
        if (patient.id == patientId) historyPatient = patient;
    }
    const QString patientName = historyPatient.name;
    dialog.setWindowTitle(QStringLiteral("检测历史 · %1").arg(patientName));
    dialog.resize(900, 460);
    auto* layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(20, 18, 20, 18);
    layout->setSpacing(12);
    auto* title = new QLabel(dialog.windowTitle(), &dialog);
    title->setProperty("role", QStringLiteral("dialogTitle"));
    layout->addWidget(title);

    auto* table = new QTableWidget(&dialog);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->verticalHeader()->hide();
    table->setShowGrid(false);
    table->setColumnCount(7);
    table->setHorizontalHeaderLabels({QStringLiteral("检测时间"), QStringLiteral("SOS (m/s)"),
                                      QStringLiteral("T 值"), QStringLiteral("Z 值"),
                                      QStringLiteral("骨强度"), QStringLiteral("操作账号"),
                                      QStringLiteral("参数组")});
    QList<MeasurementRecord> records = measurementsForPatient(patientId);
    std::sort(records.begin(), records.end(), [](const MeasurementRecord& a, const MeasurementRecord& b) {
        return a.measuredAt > b.measuredAt;
    });
    table->setRowCount(records.size());
    for (int row = 0; row < records.size(); ++row) {
        const MeasurementRecord record = withCurrentReference(records[row], historyPatient);
        const QDateTime measuredAt = parsedMeasurementDateTime(record.measuredAt);
        auto* timeItem = new QTableWidgetItem(measuredAt.isValid()
            ? measuredAt.toString(QStringLiteral("yyyy-MM-dd HH:mm")) : record.measuredAt);
        timeItem->setData(Qt::UserRole, record.id);
        table->setItem(row, 0, timeItem);
        table->setItem(row, 1, new QTableWidgetItem(record.sos));
        table->setItem(row, 2, new QTableWidgetItem(record.tScore));
        table->setItem(row, 3, new QTableWidgetItem(record.zScore));
        table->setItem(row, 4, new QTableWidgetItem(record.boneStrength.isEmpty()
                                                        ? record.diagnosis : record.boneStrength));
        table->setItem(row, 5, new QTableWidgetItem(record.operatorName));
        table->setItem(row, 6, new QTableWidgetItem(record.parameterGroup.isEmpty()
                                                        ? QStringLiteral("未记录") : record.parameterGroup));
    }
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    if (table->rowCount() > 0) table->selectRow(0);
    layout->addWidget(table, 1);

    auto* buttons = new QHBoxLayout();
    auto* reportButton = new QPushButton(QStringLiteral("查看报表"), &dialog);
    reportButton->setProperty("variant", QStringLiteral("primary"));
    auto* deleteButton = new QPushButton(QStringLiteral("删除本条记录"), &dialog);
    deleteButton->setProperty("variant", QStringLiteral("dangerOutline"));
    deleteButton->setEnabled(patientDataWritable);
    auto* closeButton = new QPushButton(QStringLiteral("关闭"), &dialog);
    buttons->addWidget(reportButton);
    buttons->addWidget(deleteButton);
    buttons->addStretch();
    buttons->addWidget(closeButton);
    layout->addLayout(buttons);
    connect(closeButton, &QPushButton::clicked, &dialog, &QDialog::accept);

    auto openSelectedReport = [this, table, patientId, &dialog]() {
        const int row = table->currentRow();
        if (row < 0 || !table->item(row, 0)) {
            QMessageBox::information(&dialog, "提示", "请先选择一条检测记录。");
            return;
        }
        const QString recordId = table->item(row, 0)->data(Qt::UserRole).toString();
        const PatientInfo* patient = nullptr;
        const MeasurementRecord* measurement = nullptr;
        for (const PatientInfo& item : patientList) {
            if (item.id == patientId) {
                patient = &item;
                break;
            }
        }
        for (const MeasurementRecord& item : measurementList) {
            if (item.id == recordId) {
                measurement = &item;
                break;
            }
        }
        if (!patient || !measurement) {
            QMessageBox::warning(&dialog, "报表", "无法找到这条检测记录对应的报表数据。");
            return;
        }
        const PatientInfo patientCopy = *patient;
        const MeasurementRecord measurementCopy = *measurement;
        dialog.accept();
        showReportFrom(ui->pageArchive, patientCopy, measurementCopy);
    };
    connect(reportButton, &QPushButton::clicked, &dialog, openSelectedReport);
    connect(table, &QTableWidget::cellDoubleClicked, &dialog,
            [openSelectedReport](int, int) { openSelectedReport(); });
    connect(deleteButton, &QPushButton::clicked, &dialog, [this, table, &dialog]() {
        const int row = table->currentRow();
        if (row < 0 || !table->item(row, 0)) return;
        const QString recordId = table->item(row, 0)->data(Qt::UserRole).toString();
        if (QMessageBox::question(&dialog, "确认删除", "确定删除这条检测记录？删除后无法恢复。")
            != QMessageBox::Yes) {
            return;
        }
        if (deleteMeasurementRecord(recordId)) table->removeRow(row);
    });
    dialog.exec();
}

QString MainWindow::selectedArchivePatientId() const
{
    const int row = ui->table->currentRow();
    if (row < 0 || !ui->table->item(row, ArchiveIdColumn)) return QString();
    if (!ui->table->selectionModel() || !ui->table->selectionModel()->isRowSelected(row, QModelIndex()))
        return QString();
    return ui->table->item(row, ArchiveIdColumn)->text();
}

void MainWindow::on_btnSelectPatient_clicked()
{
    if (patientMeasureRunning) return;
    const QString id = selectedArchivePatientId();
    if (id.isEmpty()) {
        QMessageBox::information(this, "提示", "请先在表格中选中一位被测者。");
        return;
    }
    for (const PatientInfo& patient : patientList) {
        if (patient.id == id) {
            if (!selectCurrentPatient(patient)) return;
            ui->stackedWidget->setCurrentWidget(ui->pageMain);
            scheduleResponsiveLayout();
            return;
        }
    }
}

void MainWindow::on_btnViewHistory_clicked()
{
    if (patientMeasureRunning) return;
    const QString id = selectedArchivePatientId();
    if (id.isEmpty()) {
        QMessageBox::information(this, "提示", "请先在表格中选中一位被测者。");
        return;
    }
    showPatientHistory(id);
}

void MainWindow::refreshTable(const QList<PatientInfo> &list) {
    const QString previouslySelected = ui->table->currentRow() >= 0 && ui->table->item(ui->table->currentRow(), ArchiveIdColumn)
        ? ui->table->item(ui->table->currentRow(), ArchiveIdColumn)->text() : QString();
    QSignalBlocker blocker(ui->table);
    ui->table->clear();
    ui->table->setColumnCount(ArchiveColumnCount);
    ui->table->setHorizontalHeaderLabels({QStringLiteral("编号"), QStringLiteral("姓名"),
                                          QStringLiteral("性别"), QStringLiteral("出生日期（年龄）"),
                                          QStringLiteral("最近检测"), QStringLiteral("最近 SOS"),
                                          QStringLiteral("次数")});
    ui->table->setRowCount(list.size());
    int rowToSelect = -1;
    for (int i = 0; i < list.size(); ++i) {
        const PatientInfo &p = list[i];
        auto* itemID = new QTableWidgetItem(p.id);
        itemID->setCheckState(Qt::Unchecked);
        itemID->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        ui->table->setItem(i, ArchiveIdColumn, itemID);

        const int age = ageOnDate(p.birthDay, QDate::currentDate());
        const QString birth = p.birthDay.isEmpty() ? QStringLiteral("--")
            : age >= 0 ? QStringLiteral("%1（%2）").arg(p.birthDay).arg(age) : p.birthDay;
        const MeasurementRecord* latest = latestMeasurementForPatient(p.id);
        const QDateTime latestTime = latest ? parsedMeasurementDateTime(latest->measuredAt) : QDateTime();
        const QString date = !latest ? QStringLiteral("--")
            : latestTime.isValid() ? latestTime.date().toString(QStringLiteral("yyyy-MM-dd"))
                                   : latest->measuredAt.left(10);
        const QStringList values = {p.name, p.gender.isEmpty() ? QStringLiteral("--") : p.gender, birth, date,
                                    latest ? latest->sos : QStringLiteral("--"),
                                    QString::number(measurementsForPatient(p.id).size())};
        for (int column = ArchiveNameColumn; column < ArchiveColumnCount; ++column) {
            auto* item = new QTableWidgetItem(values.at(column - ArchiveNameColumn));
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            ui->table->setItem(i, column, item);
        }
        if (p.id == previouslySelected) rowToSelect = i;
    }

    QHeaderView* header = ui->table->horizontalHeader();
    header->setStretchLastSection(false);
    header->setSectionResizeMode(QHeaderView::Stretch);
    for (int column : {ArchiveIdColumn, ArchiveGenderColumn, ArchiveSosColumn, ArchiveCountColumn})
        header->setSectionResizeMode(column, QHeaderView::ResizeToContents);
    ui->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->table->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    if (rowToSelect >= 0) ui->table->selectRow(rowToSelect);
    blocker.unblock();
    updateArchiveSelectionBar();
    updateAgeSosReference();
}

void MainWindow::on_btnShowAll_clicked() {
    ui->editSearchKeyword->clear();
    if (chkDateFilter) chkDateFilter->setChecked(false);
    refreshTable(patientList);
}

// 姓名/编号关键字，可选叠加检测日期
void MainWindow::on_btnSearchName_clicked() {
    const QString key = ui->editSearchKeyword->text().trimmed();
    const bool byDate = chkDateFilter && chkDateFilter->isChecked();
    const QString date = byDate ? dateFilter->date().toString(QStringLiteral("yyyy-MM-dd")) : QString();
    QList<PatientInfo> result;
    for (const PatientInfo& p : patientList) {
        if (!key.isEmpty() && !p.name.contains(key, Qt::CaseInsensitive) &&
            !p.id.contains(key, Qt::CaseInsensitive)) {
            continue;
        }
        if (byDate) {
            bool measuredThatDay = false;
            for (const MeasurementRecord& record : measurementsForPatient(p.id)) {
                const QDateTime time = parsedMeasurementDateTime(record.measuredAt);
                const QString day = time.isValid() ? time.date().toString(QStringLiteral("yyyy-MM-dd"))
                                                   : record.measuredAt.left(10);
                if (day == date) {
                    measuredThatDay = true;
                    break;
                }
            }
            if (!measuredThatDay) continue;
        }
        result << p;
    }
    refreshTable(result);
    if (result.isEmpty() && (!key.isEmpty() || byDate)) {
        statusBar()->showMessage(QStringLiteral("没有找到符合条件的档案。"), 5000);
    }
}

void MainWindow::on_btnDeleteSelected_clicked()
{
    if (patientMeasureRunning) {
        QMessageBox::warning(this, "检测进行中", "检测进行中不能删除档案。");
        return;
    }
    QStringList orderedIds;
    QSet<QString> patientIds;
    for (int i = 0; i < ui->table->rowCount(); ++i) {
        QTableWidgetItem *item = ui->table->item(i, ArchiveIdColumn);
        if (item && item->checkState() == Qt::Checked && !patientIds.contains(item->text())) {
            patientIds.insert(item->text());
            orderedIds << item->text();
        }
    }

    if (patientIds.isEmpty()) {
        QMessageBox::information(this, "提示", "请先勾选需要删除的档案。");
        return;
    }
    if (hasPendingMeasurement && patientIds.contains(pendingMeasurement.patientId)) {
        QMessageBox::warning(this, "存在未保存结果",
                             "当前被测者还有未保存的检测结果，请先重试保存后再删除档案。");
        return;
    }

    QStringList summaries;
    for (const QString& id : orderedIds) {
        QString name = id;
        for (const PatientInfo& patient : patientList) {
            if (patient.id == id) name = patient.name;
        }
        summaries << QStringLiteral("%1（%2 条检测记录）").arg(name).arg(measurementsForPatient(id).size());
    }
    QMessageBox box(QMessageBox::Warning, QStringLiteral("确认删除"),
                    QStringLiteral("确定删除选中的 %1 份档案吗？").arg(orderedIds.size()),
                    QMessageBox::Yes | QMessageBox::No, this);
    box.setInformativeText(summaries.join(QStringLiteral("、")) +
                           QStringLiteral("\n这些人的全部检测记录会一起删除。"
                                          "删除前会自动在软件文件夹的 backups 里留一份备份。"));
    box.setDefaultButton(QMessageBox::No);
    box.button(QMessageBox::Yes)->setText(QStringLiteral("删除"));
    box.button(QMessageBox::No)->setText(QStringLiteral("取消"));
    if (box.exec() != QMessageBox::Yes) return;

    QList<PatientInfo> patientCandidate;
    for (const PatientInfo& patient : patientList) {
        if (!patientIds.contains(patient.id)) patientCandidate.append(patient);
    }
    QList<MeasurementRecord> measurementCandidate;
    for (const MeasurementRecord& record : measurementList) {
        if (!patientIds.contains(record.patientId)) measurementCandidate.append(record);
    }
    if (!backupBeforeDelete()) return;
    if (!savePatientData(patientCandidate, measurementCandidate)) return;

    const bool removedCurrent = patientIds.contains(currentPatient.id);
    patientList = patientCandidate;
    measurementList = measurementCandidate;
    if (removedCurrent) clearCurrentPatient();
    refreshPatientDerivedViews();
    statusBar()->showMessage(QStringLiteral("已删除 %1 份档案。").arg(patientIds.size()), 5000);
}

void MainWindow::on_btnAdd_clicked() {
    if (patientMeasureRunning) {
        QMessageBox::warning(this, "检测进行中", "检测进行中不能新建档案。");
        return;
    }
    openNewPatientDialog(false);
}

void MainWindow::on_table_cellDoubleClicked(int row, int)
{
    if (patientMeasureRunning) {
        QMessageBox::warning(this, "检测进行中", "检测进行中不能更换被测者。");
        return;
    }
    if (row < 0 || row >= ui->table->rowCount()) return;
    QTableWidgetItem* idItem = ui->table->item(row, ArchiveIdColumn);
    if (!idItem) return;
    for (const PatientInfo& patient : patientList) {
        if (patient.id == idItem->text()) {
            if (!selectCurrentPatient(patient)) return;
            ui->stackedWidget->setCurrentWidget(ui->pageMain);
            scheduleResponsiveLayout();
            return;
        }
    }
    QMessageBox::warning(this, "错误", "未找到该行对应的档案。");
}

void MainWindow::updateArchiveSelectionBar()
{
    if (!lblArchiveSelection || !lblCheckedCount) return;
    const QString id = selectedArchivePatientId();
    QString name;
    for (const PatientInfo& patient : patientList) {
        if (patient.id == id) name = patient.name;
    }
    static_cast<ElidingLabel*>(lblArchiveSelection)->setFullText(id.isEmpty() ? QStringLiteral("未选择")
                                              : QStringLiteral("%1（%2）").arg(name, id));
    int checked = 0;
    for (int row = 0; row < ui->table->rowCount(); ++row) {
        if (ui->table->item(row, ArchiveIdColumn) &&
            ui->table->item(row, ArchiveIdColumn)->checkState() == Qt::Checked) {
            ++checked;
        }
    }
    lblCheckedCount->setText(QStringLiteral("已勾选 %1 人").arg(checked));
    if (btnExport) btnExport->setText(checked > 0 ? QStringLiteral("导出勾选") : QStringLiteral("导出全部"));
    const bool nextRoundPending = nextRoundTimer.isActive();
    const bool rowActionsAllowed = !id.isEmpty() && !patientMeasureRunning;
    ui->btnSelectPatient->setEnabled(rowActionsAllowed && !nextRoundPending);
    ui->btnViewHistory->setEnabled(rowActionsAllowed);
    if (btnEditPatient) btnEditPatient->setEnabled(rowActionsAllowed && patientDataWritable && !nextRoundPending);
    ui->btnDeleteSelected->setEnabled(checked > 0 && patientDataWritable && !nextRoundPending &&
                                      !patientMeasureRunning);
}

void MainWindow::openNewPatientDialog(bool makeCurrent)
{
    if (patientMeasureRunning) return;
    if (!ensurePatientDataWritable()) return;
    PatientFormDialog dialog(PatientFormDialog::Mode::Create, this);
    dialog.setSuggestedId(PatientFormDialog::suggestId(patientList, QDate::currentDate()));
    QSet<QString> ids;
    for (const PatientInfo& patient : patientList) ids.insert(patient.id);
    dialog.setExistingIds(ids);
    if (makeCurrent) dialog.setSaveButtonText(QStringLiteral("保存并设为当前被测者"));
    if (dialog.exec() != QDialog::Accepted) return;
    const PatientInfo patient = dialog.patient();
    if (!createPatient(patient, makeCurrent)) return;
    if (makeCurrent) {
        ui->stackedWidget->setCurrentWidget(ui->pageMain);
        scheduleResponsiveLayout();
        return;
    }
    for (int row = 0; row < ui->table->rowCount(); ++row) {
        if (ui->table->item(row, ArchiveIdColumn) &&
            ui->table->item(row, ArchiveIdColumn)->text() == patient.id) {
            ui->table->selectRow(row);
            break;
        }
    }
}

void MainWindow::openEditPatientDialog(const QString& patientId)
{
    if (patientMeasureRunning) return;
    for (const PatientInfo& existing : patientList) {
        if (existing.id != patientId) continue;
        PatientFormDialog dialog(PatientFormDialog::Mode::Edit, this);
        dialog.setPatient(existing);
        QSet<QString> ids;
        for (const PatientInfo& patient : patientList) ids.insert(patient.id);
        dialog.setExistingIds(ids);
        if (dialog.exec() == QDialog::Accepted) updatePatient(dialog.patient());
        return;
    }
}

bool MainWindow::createPatient(const PatientInfo& patient, bool makeCurrent)
{
    if (patientMeasureRunning) return false;
    if (patient.name.trimmed().isEmpty() || patient.id.trimmed().isEmpty()) {
        QMessageBox::warning(this, "缺少信息", "姓名和编号不能为空。");
        return false;
    }
    for (const PatientInfo& existing : patientList) {
        if (existing.id == patient.id) {
            QMessageBox::warning(this, "重复编号", "该编号已存在，请从档案中选择被测者。");
            return false;
        }
    }
    if (makeCurrent && !confirmPatientChange(patient.id)) return false;

    QList<PatientInfo> candidate = patientList;
    candidate.append(patient);
    if (!savePatients(candidate)) return false;
    patientList = candidate;
    if (makeCurrent) applyCurrentPatient(patient);
    refreshPatientDerivedViews();
    return true;
}

bool MainWindow::updatePatient(const PatientInfo& patient)
{
    if (patientMeasureRunning) {
        QMessageBox::warning(this, "检测进行中", "检测进行中不能编辑档案。");
        return false;
    }
    int index = -1;
    for (int i = 0; i < patientList.size(); ++i) {
        if (patientList[i].id == patient.id) {
            index = i;
            break;
        }
    }
    if (index < 0) {
        QMessageBox::warning(this, "错误", "没有找到这份档案。");
        return false;
    }
    if (patient.name.trimmed().isEmpty()) {
        QMessageBox::warning(this, "缺少信息", "姓名不能为空。");
        return false;
    }
    QList<PatientInfo> candidate = patientList;
    candidate[index] = patient;
    if (!savePatients(candidate)) return false;
    patientList = candidate;
    if (currentPatient.id == patient.id) {
        // Only the editable archive fields; session-only fields stay untouched.
        currentPatient.name = patient.name;
        currentPatient.gender = patient.gender;
        currentPatient.birthDay = patient.birthDay;
        currentPatient.height = patient.height;
        currentPatient.weight = patient.weight;
    }
    refreshPatientDerivedViews();
    statusBar()->showMessage(QStringLiteral("档案已保存。"), 4000);
    return true;
}

bool MainWindow::deleteMeasurementRecord(const QString& recordId)
{
    for (int i = 0; i < measurementList.size(); ++i) {
        if (measurementList[i].id != recordId) continue;
        QList<MeasurementRecord> candidate = measurementList;
        candidate.removeAt(i);
        if (!backupBeforeDelete()) return false;
        if (!saveMeasurements(candidate)) return false;
        measurementList = candidate;
        refreshPatientDerivedViews();
        return true;
    }
    return false;
}

// ==================== 导出检测记录 ====================

namespace {

QString csvField(const QString& value)
{
    if (!value.contains(QLatin1Char(',')) && !value.contains(QLatin1Char('"')) &&
        !value.contains(QLatin1Char('\n')) && !value.contains(QLatin1Char('\r'))) {
        return value;
    }
    QString quoted = value;
    quoted.replace(QLatin1Char('"'), QStringLiteral("\"\""));
    return QLatin1Char('"') + quoted + QLatin1Char('"');
}

} // namespace

QString MainWindow::measurementsCsv(const QList<PatientInfo>& patients,
                                    const QList<MeasurementRecord>& measurements)
{
    const QStringList header = {
        QStringLiteral("档案编号"), QStringLiteral("姓名"), QStringLiteral("性别"),
        QStringLiteral("出生日期"), QStringLiteral("检测时间"), QStringLiteral("检测时年龄"),
        QStringLiteral("部位"), QStringLiteral("SOS(m/s)"), QStringLiteral("T值"),
        QStringLiteral("Z值"), QStringLiteral("骨强度"), QStringLiteral("相对骨折风险"),
        QStringLiteral("相对骨龄"), QStringLiteral("诊断提示"), QStringLiteral("操作账号"),
        QStringLiteral("身高(cm)"), QStringLiteral("体重(kg)"), QStringLiteral("参数组")};
    const QString lineEnd = QStringLiteral("\r\n");
    QString csv = header.join(QLatin1Char(',')) + lineEnd;

    QList<MeasurementRecord> rows;
    for (const MeasurementRecord& record : measurements) {
        for (const PatientInfo& patient : patients) {
            if (patient.id == record.patientId) {
                rows.append(record);
                break;
            }
        }
    }
    std::stable_sort(rows.begin(), rows.end(), [](const MeasurementRecord& a, const MeasurementRecord& b) {
        return a.patientId != b.patientId ? a.patientId < b.patientId : a.measuredAt < b.measuredAt;
    });

    const auto snapshotOr = [](const QString& snapshot, const QString& current) {
        return snapshot.trimmed().isEmpty() ? current : snapshot;
    };
    for (const MeasurementRecord& saved : rows) {
        PatientInfo patient;
        for (const PatientInfo& candidate : patients) {
            if (candidate.id == saved.patientId) patient = candidate;
        }
        const MeasurementRecord record = withCurrentReference(saved, patient);
        const QDateTime time = parsedMeasurementDateTime(record.measuredAt);
        const int age = measurementAge(record, patient);
        const QStringList fields = {
            record.patientId,
            snapshotOr(record.patientName, patient.name),
            measurementGender(record, patient),
            snapshotOr(record.patientBirthDay, patient.birthDay),
            time.isValid() ? time.toString(QStringLiteral("yyyy-MM-dd HH:mm")) : record.measuredAt,
            age >= 0 ? QString::number(age) : QString(),
            record.part,
            record.sos,
            record.tScore,
            record.zScore,
            record.boneStrength,
            record.fractureRisk,
            record.boneAge,
            record.diagnosis == record.boneStrength ? QString() : record.diagnosis,
            record.operatorName,
            snapshotOr(record.patientHeight, patient.height),
            snapshotOr(record.patientWeight, patient.weight),
            record.parameterGroup.isEmpty() ? QStringLiteral("未记录") : record.parameterGroup};
        QStringList escaped;
        for (const QString& field : fields) escaped << csvField(field.trimmed());
        csv += escaped.join(QLatin1Char(',')) + lineEnd;
    }
    return csv;
}

QStringList MainWindow::checkedArchivePatientIds() const
{
    QStringList ids;
    for (int row = 0; row < ui->table->rowCount(); ++row) {
        const QTableWidgetItem* item = ui->table->item(row, ArchiveIdColumn);
        if (item && item->checkState() == Qt::Checked && !ids.contains(item->text())) ids << item->text();
    }
    return ids;
}

void MainWindow::exportMeasurements()
{
    const QStringList checkedIds = checkedArchivePatientIds();
    QList<PatientInfo> patients;
    for (const PatientInfo& patient : patientList) {
        if (checkedIds.isEmpty() || checkedIds.contains(patient.id)) patients.append(patient);
    }
    int recordCount = 0;
    for (const MeasurementRecord& record : measurementList) {
        for (const PatientInfo& patient : patients) {
            if (patient.id == record.patientId) {
                ++recordCount;
                break;
            }
        }
    }
    if (recordCount == 0) {
        QMessageBox::information(this, QStringLiteral("导出数据"),
                                 checkedIds.isEmpty() ? QStringLiteral("还没有任何检测记录。")
                                                      : QStringLiteral("勾选的档案还没有检测记录。"));
        return;
    }

    const QString documents = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    const QString defaultPath = QDir(documents).filePath(
        QStringLiteral("骨密度检测记录_%1.csv").arg(QDate::currentDate().toString(QStringLiteral("yyyyMMdd"))));
    const QString path = QFileDialog::getSaveFileName(
        this, checkedIds.isEmpty() ? QStringLiteral("导出全部检测记录") : QStringLiteral("导出勾选档案的检测记录"),
        defaultPath, QStringLiteral("CSV 表格 (*.csv)"));
    if (path.isEmpty()) return;

    QSaveFile file(path);
    QByteArray bytes("\xEF\xBB\xBF");   // UTF-8 BOM so Excel shows Chinese correctly
    bytes += measurementsCsv(patients, measurementList).toUtf8();
    if (!file.open(QIODevice::WriteOnly) || file.write(bytes) != bytes.size() || !file.commit()) {
        QMessageBox::warning(this, QStringLiteral("导出失败"),
                             QStringLiteral("无法写入文件，请换一个保存位置后重试。"));
        return;
    }
    statusBar()->showMessage(QStringLiteral("已导出 %1 人、%2 条检测记录。").arg(patients.size()).arg(recordCount), 6000);
}

// ==================== 数据备份 ====================

QString MainWindow::backupRoot() const
{
    return QFileInfo(xmlFilePath).absoluteDir().filePath(QStringLiteral("backups"));
}

QStringList MainWindow::dataFilePaths() const
{
    return {xmlFilePath, measurementsFilePath, accountsFilePath, calibrationFilePath};
}

void MainWindow::backupDataDaily()
{
    const DataBackup::Result result = DataBackup::snapshot(
        backupRoot(), dataFilePaths(), DataBackup::dailyLabel(QDate::currentDate()));
    if (!result.ok) {
        statusBar()->showMessage(
            QStringLiteral("今日自动备份失败：%1。数据仍可正常使用。").arg(result.error), 10000);
        return;
    }
    if (result.created) DataBackup::prune(backupRoot());
}

bool MainWindow::backupBeforeDelete()
{
    const DataBackup::Result result = DataBackup::snapshot(
        backupRoot(), dataFilePaths(), DataBackup::beforeDeleteLabel(QDateTime::currentDateTime()));
    if (result.ok) {
        DataBackup::prune(backupRoot());
        return true;
    }
    return QMessageBox::warning(
               this, QStringLiteral("删除前备份失败"),
               QStringLiteral("%1。\n\n仍要删除吗？删除后将无法从备份恢复。").arg(result.error),
               QMessageBox::Yes | QMessageBox::No, QMessageBox::No)
        == QMessageBox::Yes;
}
