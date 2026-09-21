#pragma once

#include <QObject>
#include <QDomDocument>
#include <QList>
#include <QString>

// 简单数据结构，存患者信息
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

class PatientManager : public QObject {
    Q_OBJECT
public:
    explicit PatientManager(QObject *parent = nullptr);

    // load existing XML into internal QDomDocument (replace current)
    bool loadFile(const QString &filePath);

    // save current QDomDocument to file
    bool saveFile(const QString &filePath);

    // access helpers
    QList<PatientInfo> allPatients() const;

    // modify in-memory document
    void addPatient(const PatientInfo &info);
    void updatePatient(int index, const PatientInfo &info);
    void removePatient(int index);

private:
    QDomDocument doc; // 内部 DOM 文档
};
