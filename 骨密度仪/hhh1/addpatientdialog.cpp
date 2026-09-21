#include "addpatientdialog.h"
#include "ui_addpatientdialog.h"   // Qt Designer 生成的头文件
#include <QDateTime>

AddPatientDialog::AddPatientDialog(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::AddPatientDialog)
{
    ui->setupUi(this);
    setWindowTitle("新增患者信息");
}

AddPatientDialog::~AddPatientDialog()
{
    delete ui;
}

PatientInfo AddPatientDialog::patient() const
{
    PatientInfo info;
    info.id        = ui->editId->text();
    info.name      = ui->editName->text();
    info.gender    = ui->comboGender->currentText();
    info.birthDay  = ui->editBirth->text();
    info.checkPart = ui->editPart->text();
    info.height    = ui->editHeight->text();
    info.weight    = ui->editWeight->text();
    info.diagprompt= ui->editDiag->text();
    info.checkTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    return info;
}
