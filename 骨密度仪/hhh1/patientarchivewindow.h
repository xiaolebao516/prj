#ifndef PATIENTARCHIVEWINDOW_H
#define PATIENTARCHIVEWINDOW_H

#include <QWidget>
#include <QDomDocument>
#include <QList>

namespace Ui { class PatientArchiveWindow; }

struct PatientInfo {
    QString id;
    QString name;
    QString gender;
    QString birthDay;
    QString checkPart;
    QString checkTime;
    QString height;
    QString weight;
    QString diagprompt;
};

class PatientArchiveWindow : public QWidget
{
    Q_OBJECT
public:
    explicit PatientArchiveWindow(const QString &xmlPath, QWidget *parent = nullptr);
    ~PatientArchiveWindow();

signals:
    void backToMain(); // 通知主界面返回

private slots:
    void onBtnBack();
    void onBtnShowAll();
    void onBtnSearchName();
    void onBtnSearchID();
    void onBtnSearchDate();
    void onBtnAdd();
    void onTableDoubleClicked(int row, int col);

private:
    Ui::PatientArchiveWindow *ui;
    QString m_xmlPath;
    QDomDocument m_doc;

    QList<PatientInfo> loadPatients() const;
    void savePatients(const QList<PatientInfo> &list);
    void refreshTableFromList(const QList<PatientInfo> &list);
    void refreshTable(); // reload file -> table
    void showPatientDetailDialog(const PatientInfo &p, int rowIndex);
};

#endif // PATIENTARCHIVEWINDOW_H
