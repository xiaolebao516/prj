#include "accountstore.h"
#include <QCryptographicHash>
#include <QDomDocument>
#include <QFile>
#include <QRandomGenerator>
#include <QSaveFile>
#include <QTextStream>

QString AccountStore::hashPassword(const QString& salt, const QString& password) const
{
    return QCryptographicHash::hash((salt + password).toUtf8(), QCryptographicHash::Sha256).toBase64();
}

bool AccountStore::loadOrInitialize(const QString& path, QString* errorMessage)
{
    path_ = path;
    QFile file(path);
    if (!file.exists()) {
        AccountInfo admin;
        admin.username = "admin"; admin.role = "admin";
        admin.salt = QString::number(QRandomGenerator::global()->generate64(), 16);
        admin.passwordHash = hashPassword(admin.salt, "admin123");
        accounts_ = {admin};
        return save(errorMessage);
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) { if (errorMessage) *errorMessage=file.errorString(); return false; }
    QDomDocument doc;
    if (!doc.setContent(&file)) { if (errorMessage) *errorMessage="accounts.xml 格式错误"; return false; }
    accounts_.clear();
    const QDomNodeList nodes=doc.documentElement().elementsByTagName("account");
    for (int i=0;i<nodes.count();++i) { QDomElement e=nodes.at(i).toElement(); AccountInfo a; a.username=e.attribute("username"); a.role=e.attribute("role"); a.enabled=e.attribute("enabled","true")=="true"; a.salt=e.attribute("salt"); a.passwordHash=e.attribute("passwordHash"); if(!a.username.isEmpty()) accounts_.append(a); }
    return true;
}

bool AccountStore::authenticate(const QString& username, const QString& password, AccountInfo* account) const
{ for (const AccountInfo& a:accounts_) if (a.username==username && a.enabled && a.passwordHash==hashPassword(a.salt,password)) { if(account)*account=a; return true; } return false; }
bool AccountStore::save(QString* errorMessage) const
{ QDomDocument doc; QDomElement root=doc.createElement("accounts"); root.setAttribute("version","1"); doc.appendChild(root); for(const AccountInfo&a:accounts_){QDomElement e=doc.createElement("account");e.setAttribute("username",a.username);e.setAttribute("role",a.role);e.setAttribute("enabled",a.enabled?"true":"false");e.setAttribute("salt",a.salt);e.setAttribute("passwordHash",a.passwordHash);root.appendChild(e);} QSaveFile file(path_); if(!file.open(QIODevice::WriteOnly|QIODevice::Text)){if(errorMessage)*errorMessage=file.errorString();return false;} QTextStream out(&file);doc.save(out,4);if(!file.commit()){if(errorMessage)*errorMessage=file.errorString();return false;}return true; }
bool AccountStore::createUser(const QString& username,const QString& password,QString* errorMessage){if(username.trimmed().isEmpty()||password.isEmpty()){if(errorMessage)*errorMessage="账号和密码不能为空";return false;}for(const AccountInfo&a:accounts_)if(a.username==username){if(errorMessage)*errorMessage="账号已存在";return false;}AccountInfo a;a.username=username.trimmed();a.role="user";a.salt=QString::number(QRandomGenerator::global()->generate64(),16);a.passwordHash=hashPassword(a.salt,password);accounts_.append(a);if(!save(errorMessage)){accounts_.removeLast();return false;}return true;}
bool AccountStore::setEnabled(const QString& username,bool enabled,QString* errorMessage){for(AccountInfo&a:accounts_)if(a.username==username){if(a.role=="admin"&&!enabled){if(errorMessage)*errorMessage="不能停用管理员";return false;}a.enabled=enabled;return save(errorMessage);}return false;}
bool AccountStore::resetPassword(const QString& username,const QString& password,QString* errorMessage){if(password.isEmpty()){if(errorMessage)*errorMessage="密码不能为空";return false;}for(AccountInfo&a:accounts_)if(a.username==username){a.salt=QString::number(QRandomGenerator::global()->generate64(),16);a.passwordHash=hashPassword(a.salt,password);return save(errorMessage);}return false;}
bool AccountStore::deleteUser(const QString& username,QString* errorMessage){for(int i=0;i<accounts_.size();++i)if(accounts_[i].username==username){if(accounts_[i].role=="admin"){if(errorMessage)*errorMessage="不能删除管理员";return false;}accounts_.removeAt(i);return save(errorMessage);}return false;}
