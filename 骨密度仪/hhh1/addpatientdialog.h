#ifndef ADDPATIENTDIALOG_H
#define ADDPATIENTDIALOG_H

#pragma once
#include <QDialog>
#include "patientmanager.h"   // 用到 PatientInfo 结构体

namespace Ui {
class AddPatientDialog;
}

class AddPatientDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddPatientDialog(QWidget *parent = nullptr);
    ~AddPatientDialog();

    PatientInfo patient() const;   // 获取填写好的患者信息

private:
    Ui::AddPatientDialog *ui;
};


#endif // ADDPATIENTDIALOG_H
