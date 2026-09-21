#include "patientmanager.h"

#include <QDomElement>
#include <QFile>
#include <QTextStream>
#include <QDebug>

PatientManager::PatientManager(QObject *parent)
    : QObject(parent)
{
    // 默认可选：初始化空文档根
    // doc.appendChild(doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""));
}

bool PatientManager::loadFile(const QString &filePath)
{
    QFile f(filePath);
    if (!f.exists()) {
        // 文件不存在，建立一个空的根供后续添加
        doc.clear();
        QDomElement root = doc.createElement("subInfo");
        doc.appendChild(root);
        return true;
    }
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning() << "PatientManager::loadFile: cannot open" << filePath;
        return false;
    }
    QString errMsg; int errLine = 0, errCol = 0;
    if (!doc.setContent(&f, &errMsg, &errLine, &errCol)) {
        qWarning() << "PatientManager::loadFile: parse error" << errMsg << "line" << errLine << "col" << errCol;
        f.close();
        return false;
    }
    f.close();
    return true;
}

bool PatientManager::saveFile(const QString &filePath)
{
    QFile f(filePath);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "PatientManager::saveFile: cannot open" << filePath;
        return false;
    }
    QTextStream out(&f);
    doc.save(out, 4); // 缩进 4 空格
    f.close();
    return true;
}

QList<PatientInfo> PatientManager::allPatients() const
{
    QList<PatientInfo> list;
    QDomElement root = doc.documentElement();
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

void PatientManager::addPatient(const PatientInfo &info)
{
    QDomElement root = doc.documentElement();
    if (root.isNull()) {
        root = doc.createElement("subInfo");
        doc.appendChild(root);
    }

    QDomElement check = doc.createElement("checkInfo");

    auto appendTextElement = [&](const QString &tag, const QString &value) {
        QDomElement el = doc.createElement(tag);
        QDomText text = doc.createTextNode(value);
        el.appendChild(text);
        check.appendChild(el);
    };

    appendTextElement("id", info.id);
    appendTextElement("name", info.name);
    appendTextElement("gender", info.gender);
    appendTextElement("birthDay", info.birthDay);
    appendTextElement("checkPart", info.checkPart);
    appendTextElement("checkTime", info.checkTime);
    appendTextElement("height", info.height);
    appendTextElement("weight", info.weight);
    appendTextElement("diagprompt", info.diagprompt);

    root.appendChild(check);
}

void PatientManager::updatePatient(int index, const PatientInfo &info)
{
    QDomElement root = doc.documentElement();
    if (root.isNull()) return;
    QDomNodeList nodes = root.elementsByTagName("checkInfo");
    if (index < 0 || index >= nodes.count()) return;
    QDomElement e = nodes.at(index).toElement();

    auto setText = [&](const QString &tag, const QString &val){
        QDomElement child = e.firstChildElement(tag);
        if (child.isNull()) {
            QDomElement newc = doc.createElement(tag);
            newc.appendChild(doc.createTextNode(val));
            e.appendChild(newc);
        } else {
            // 如果该子元素包含文本节点，设置它；否则创建文本节点
            if (child.firstChild().isNull()) {
                child.appendChild(doc.createTextNode(val));
            } else {
                child.firstChild().setNodeValue(val);
            }
        }
    };

    setText("id", info.id);
    setText("name", info.name);
    setText("gender", info.gender);
    setText("birthDay", info.birthDay);
    setText("checkPart", info.checkPart);
    setText("checkTime", info.checkTime);
    setText("height", info.height);
    setText("weight", info.weight);
    setText("diagprompt", info.diagprompt);
}

void PatientManager::removePatient(int index)
{
    QDomElement root = doc.documentElement();
    if (root.isNull()) return;
    QDomNodeList nodes = root.elementsByTagName("checkInfo");
    if (index < 0 || index >= nodes.count()) return;
    root.removeChild(nodes.at(index));
}
