#include "patientarchivewindow.h"
#include "ui_PatientArchiveWindow.h" // 由 uic 生成，务必保证 .ui 的 class = PatientArchiveWindow

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QInputDialog>
#include <QDateTime>
#include <QTableWidgetItem>
#include <QHeaderView>

PatientArchiveWindow::PatientArchiveWindow(const QString &xmlPath, QWidget *parent)
    : QWidget(parent),
    ui(new Ui::PatientArchiveWindow),
    m_xmlPath(xmlPath)
{
    ui->setupUi(this);

    // 初始化表格（与 ui 的 tableWidget 一致）
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setHorizontalHeaderLabels({"姓名", "编号", "出生日期"});
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 连接信号
    connect(ui->btnBack, &QPushButton::clicked, this, &PatientArchiveWindow::onBtnBack);
    connect(ui->btnShowAll, &QPushButton::clicked, this, &PatientArchiveWindow::onBtnShowAll);
    connect(ui->btnSearchName, &QPushButton::clicked, this, &PatientArchiveWindow::onBtnSearchName);
    connect(ui->btnSearchID, &QPushButton::clicked, this, &PatientArchiveWindow::onBtnSearchID);
    connect(ui->btnSearchDate, &QPushButton::clicked, this, &PatientArchiveWindow::onBtnSearchDate);
    connect(ui->btnAdd, &QPushButton::clicked, this, &PatientArchiveWindow::onBtnAdd);
    connect(ui->tableWidget, &QTableWidget::cellDoubleClicked, this, &PatientArchiveWindow::onTableDoubleClicked);

    // 加载 XML（如果不存在则创建根）
    QFile f(m_xmlPath);
    if (!f.exists()) {
        QDomElement root = m_doc.createElement("Patients");
        m_doc.appendChild(root);
        savePatients(QList<PatientInfo>()); // 创建空文件
    } else {
        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            if (!m_doc.setContent(&f)) {
                QMessageBox::warning(this, "警告", "XML 解析失败，重建空数据库");
                m_doc.clear();
                m_doc.appendChild(m_doc.createElement("Patients"));
            }
            f.close();
        } else {
            QMessageBox::warning(this, "警告", "无法打开 XML 文件: " + f.errorString());
            m_doc.clear();
            m_doc.appendChild(m_doc.createElement("Patients"));
        }
    }

    refreshTable();
}

PatientArchiveWindow::~PatientArchiveWindow()
{
    delete ui;
}

QList<PatientInfo> PatientArchiveWindow::loadPatients() const
{
    QList<PatientInfo> list;
    QDomElement root = m_doc.documentElement();
    if (root.isNull()) return list;
    QDomNodeList nodes = root.elementsByTagName("checkInfo");
    for (int i = 0; i < nodes.count(); ++i) {
        QDomElement e = nodes.at(i).toElement();
        PatientInfo p;
        p.id         = e.firstChildElement("id").text();
        p.name       = e.firstChildElement("name").text();
        p.gender     = e.firstChildElement("gender").text();
        p.birthDay   = e.firstChildElement("birthDay").text();
        p.checkPart  = e.firstChildElement("checkPart").text();
        p.checkTime  = e.firstChildElement("checkTime").text();
        p.height     = e.firstChildElement("height").text();
        p.weight     = e.firstChildElement("weight").text();
        p.diagprompt = e.firstChildElement("diagprompt").text();
        list << p;
    }
    return list;
}

void PatientArchiveWindow::savePatients(const QList<PatientInfo> &list)
{
    QDomDocument newDoc;
    QDomElement root = newDoc.createElement("Patients");
    newDoc.appendChild(root);

    for (const PatientInfo &p : list) {
        QDomElement check = newDoc.createElement("checkInfo");
        auto addNode = [&](const QString &tag, const QString &val){
            QDomElement el = newDoc.createElement(tag);
            el.appendChild(newDoc.createTextNode(val));
            check.appendChild(el);
        };
        addNode("id", p.id);
        addNode("name", p.name);
        addNode("gender", p.gender);
        addNode("birthDay", p.birthDay);
        addNode("checkPart", p.checkPart);
        addNode("checkTime", p.checkTime);
        addNode("height", p.height);
        addNode("weight", p.weight);
        addNode("diagprompt", p.diagprompt);
        root.appendChild(check);
    }

    QFile f(m_xmlPath);
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QTextStream out(&f);
        newDoc.save(out, 4);
        f.close();
        m_doc = newDoc;
    } else {
        QMessageBox::critical(this, "错误", "无法写入 XML 文件: " + f.errorString());
    }
}

void PatientArchiveWindow::refreshTableFromList(const QList<PatientInfo> &list)
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(list.size());
    for (int i = 0; i < list.size(); ++i) {
        const PatientInfo &p = list[i];
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(p.name));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(p.id));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(p.birthDay));
    }
}

void PatientArchiveWindow::refreshTable()
{
    // 重新从文件加载（避免搜索污染）
    QFile f(m_xmlPath);
    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (!m_doc.setContent(&f)) {
            m_doc.clear();
            m_doc.appendChild(m_doc.createElement("Patients"));
        }
        f.close();
    }
    refreshTableFromList(loadPatients());
}

void PatientArchiveWindow::onBtnBack()
{
    emit backToMain();
}

void PatientArchiveWindow::onBtnShowAll()
{
    refreshTable();
}

void PatientArchiveWindow::onBtnSearchName()
{
    bool ok=false;
    QString key = QInputDialog::getText(this, "按姓名搜索", "姓名或关键词：", QLineEdit::Normal, "", &ok);
    if (!ok || key.isEmpty()) return;

    QList<PatientInfo> found;
    for (const PatientInfo &p : loadPatients()) {
        if (p.name.contains(key, Qt::CaseInsensitive)) found << p;
    }
    if (found.isEmpty()) {
        QMessageBox::information(this, "结果", "未找到匹配的患者");
        return;
    }
    if (found.size() == 1) {
        showPatientDetailDialog(found.first(), /*rowIndex=*/-1);
    } else {
        // 多个结果：让用户选择一个
        QStringList choices;
        for (const PatientInfo &p : found) choices << QString("%1 (%2)").arg(p.name).arg(p.id);
        bool ok2=false;
        QString choice = QInputDialog::getItem(this, "选择患者", "匹配到多位患者，请选择：", choices, 0, false, &ok2);
        if (!ok2) return;
        int idx = choices.indexOf(choice);
        if (idx>=0) showPatientDetailDialog(found[idx], -1);
    }
}

void PatientArchiveWindow::onBtnSearchID()
{
    bool ok=false;
    QString key = QInputDialog::getText(this, "按编号搜索", "编号或关键词：", QLineEdit::Normal, "", &ok);
    if (!ok || key.isEmpty()) return;

    QList<PatientInfo> found;
    for (const PatientInfo &p : loadPatients()) {
        if (p.id.contains(key, Qt::CaseInsensitive)) found << p;
    }
    if (found.isEmpty()) {
        QMessageBox::information(this, "结果", "未找到匹配的患者");
        return;
    }
    if (found.size() == 1) {
        showPatientDetailDialog(found.first(), -1);
    } else {
        QStringList choices;
        for (const PatientInfo &p : found) choices << QString("%1 (%2)").arg(p.name).arg(p.id);
        bool ok2=false;
        QString choice = QInputDialog::getItem(this, "选择患者", "匹配到多位患者，请选择：", choices, 0, false, &ok2);
        if (!ok2) return;
        int idx = choices.indexOf(choice);
        if (idx>=0) showPatientDetailDialog(found[idx], -1);
    }
}

void PatientArchiveWindow::onBtnSearchDate()
{
    bool ok=false;
    QString key = QInputDialog::getText(this, "按检查日期搜索", "日期或关键词（例如 2023-05）：", QLineEdit::Normal, "", &ok);
    if (!ok || key.isEmpty()) return;

    QList<PatientInfo> found;
    for (const PatientInfo &p : loadPatients()) {
        if (p.checkTime.contains(key, Qt::CaseInsensitive)) found << p;
    }
    if (found.isEmpty()) {
        QMessageBox::information(this, "结果", "未找到匹配的患者");
        return;
    }
    if (found.size() == 1) {
        showPatientDetailDialog(found.first(), -1);
    } else {
        QStringList choices;
        for (const PatientInfo &p : found) choices << QString("%1 (%2)").arg(p.name).arg(p.checkTime);
        bool ok2=false;
        QString choice = QInputDialog::getItem(this, "选择患者", "匹配到多位患者，请选择：", choices, 0, false, &ok2);
        if (!ok2) return;
        int idx = choices.indexOf(choice);
        if (idx>=0) showPatientDetailDialog(found[idx], -1);
    }
}

void PatientArchiveWindow::onBtnAdd()
{
    PatientInfo p;
    bool ok=false;
    p.name = QInputDialog::getText(this, "新增患者", "姓名：", QLineEdit::Normal, "", &ok);
    if (!ok || p.name.isEmpty()) return;
    p.id = QInputDialog::getText(this, "新增患者", "编号（唯一）：", QLineEdit::Normal, "", &ok);
    if (!ok) return;
    p.gender = QInputDialog::getItem(this, "新增患者", "性别：", {"男","女"}, 0, false, &ok);
    if (!ok) return;
    p.birthDay = QInputDialog::getText(this, "新增患者", "出生日期（YYYY-MM-DD）：", QLineEdit::Normal, "", &ok);
    if (!ok) return;
    p.checkPart = QInputDialog::getText(this, "新增患者", "检查部位：", QLineEdit::Normal, "", &ok);
    p.checkTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    p.height = QInputDialog::getText(this, "新增患者", "身高(cm)：", QLineEdit::Normal, "", &ok);
    p.weight = QInputDialog::getText(this, "新增患者", "体重(kg)：", QLineEdit::Normal, "", &ok);
    p.diagprompt = QInputDialog::getText(this, "新增患者", "诊断提示：", QLineEdit::Normal, "", &ok);

    QList<PatientInfo> list = loadPatients();
    list << p;
    savePatients(list);
    refreshTable();
    QMessageBox::information(this, "完成", "已新增患者并保存到 XML。");
}

void PatientArchiveWindow::onTableDoubleClicked(int row, int)
{
    QList<PatientInfo> list = loadPatients();
    if (row < 0 || row >= list.size()) return;
    showPatientDetailDialog(list[row], row);
}

void PatientArchiveWindow::showPatientDetailDialog(const PatientInfo &p, int rowIndex)
{
    // 简单用对话框显示并提供修改/删除选项
    QString detail = QString("姓名：%1\n编号：%2\n性别：%3\n出生日期：%4\n检查部位：%5\n检查时间：%6\n身高：%7\n体重：%8\n诊断提示：%9")
                         .arg(p.name).arg(p.id).arg(p.gender).arg(p.birthDay).arg(p.checkPart).arg(p.checkTime)
                         .arg(p.height).arg(p.weight).arg(p.diagprompt);

    QStringList opts = {"修改", "删除", "关闭"};
    bool ok=false;
    QString choice = QInputDialog::getItem(this, "患者详情", detail + "\n\n选择操作：", opts, 2, false, &ok);
    if (!ok) return;
    if (choice == "修改") {
        // 简化：只允许修改诊断提示与身高体重等，你也可以做完整表单Dialog
        PatientInfo modified = p;
        modified.height = QInputDialog::getText(this, "修改", "身高(cm)：", QLineEdit::Normal, p.height, &ok);
        if (!ok) return;
        modified.weight = QInputDialog::getText(this, "修改", "体重(kg)：", QLineEdit::Normal, p.weight, &ok);
        if (!ok) return;
        modified.diagprompt = QInputDialog::getText(this, "修改", "诊断提示：", QLineEdit::Normal, p.diagprompt, &ok);
        if (!ok) return;

        // 替换并保存
        QList<PatientInfo> list = loadPatients();
        if (rowIndex >= 0 && rowIndex < list.size()) {
            list[rowIndex] = modified;
            savePatients(list);
            refreshTable();
            QMessageBox::information(this, "完成", "已保存修改。");
        } else {
            // 若 rowIndex 未给出，按 id 匹配替换第一个匹配项
            for (int i=0;i<list.size();++i) {
                if (list[i].id == p.id) { list[i] = modified; savePatients(list); refreshTable(); break; }
            }
            QMessageBox::information(this, "完成", "已保存修改（按 id 匹配）。");
        }
    } else if (choice == "删除") {
        if (QMessageBox::question(this, "确认", "确定要删除该患者吗？") == QMessageBox::Yes) {
            QList<PatientInfo> list = loadPatients();
            if (rowIndex >= 0 && rowIndex < list.size()) {
                list.removeAt(rowIndex);
            } else {
                // 按 id 删除第一个匹配
                for (int i=0;i<list.size();++i) {
                    if (list[i].id == p.id) { list.removeAt(i); break; }
                }
            }
            savePatients(list);
            refreshTable();
            QMessageBox::information(this, "完成", "已删除患者记录。");
        }
    } else {
        // 关闭
    }
}
