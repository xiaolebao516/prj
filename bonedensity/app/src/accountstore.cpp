#include "accountstore.h"

#include <QCryptographicHash>
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QRandomGenerator>
#include <QSaveFile>
#include <QTextStream>

namespace {

QString nextBackupPath(const QString& path)
{
    QString backupPath = path + ".bad.bak";
    int suffix = 1;
    while (QFileInfo::exists(backupPath)) {
        backupPath = path + QString(".bad.%1.bak").arg(suffix++);
    }
    return backupPath;
}

bool hasValidEnabledAdmin(const QList<AccountInfo>& accounts)
{
    for (const AccountInfo& account : accounts) {
        if (account.role != "admin" || !account.enabled || account.username.trimmed().isEmpty() ||
            account.salt.isEmpty() || account.passwordHash.isEmpty()) {
            continue;
        }
        const QByteArray salt = QByteArray::fromBase64(account.salt.toLatin1());
        const QByteArray passwordHash = QByteArray::fromBase64(account.passwordHash.toLatin1());
        if (!salt.isEmpty() &&
            passwordHash.size() == QCryptographicHash::hashLength(QCryptographicHash::Sha256)) return true;
    }
    return false;
}

} // namespace

QString AccountStore::hashPassword(const QString& salt, const QString& password) const
{
    return QCryptographicHash::hash((salt + password).toUtf8(), QCryptographicHash::Sha256).toBase64();
}

bool AccountStore::loadOrInitialize(const QString& path, QString* errorMessage)
{
    path_ = path;
    QFile file(path);
    if (!file.exists()) return resetToDefault(errorMessage);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (errorMessage) *errorMessage = file.errorString();
        return false;
    }

    QDomDocument document;
    const bool parsed = static_cast<bool>(document.setContent(&file));
    file.close();

    QList<AccountInfo> loaded;
    if (parsed && document.documentElement().tagName() == "accounts") {
        const QDomNodeList nodes = document.documentElement().elementsByTagName("account");
        for (int i = 0; i < nodes.count(); ++i) {
            const QDomElement element = nodes.at(i).toElement();
            AccountInfo account;
            account.username = element.attribute("username").trimmed();
            account.role = element.attribute("role");
            account.enabled = element.attribute("enabled", "true") == "true";
            account.salt = element.attribute("salt");
            account.passwordHash = element.attribute("passwordHash");
            if (!account.username.isEmpty()) loaded.append(account);
        }
    }

    if (!parsed || !hasValidEnabledAdmin(loaded)) {
        if (!QFile::copy(path_, nextBackupPath(path_))) {
            if (errorMessage) *errorMessage = "accounts.xml 异常且备份失败";
            return false;
        }
        return resetToDefault(errorMessage);
    }

    accounts_ = loaded;
    return true;
}

bool AccountStore::resetToDefault(QString* errorMessage)
{
    AccountInfo admin;
    admin.username = "admin";
    admin.role = "admin";
    admin.salt = QString::number(QRandomGenerator::global()->generate64(), 16);
    admin.passwordHash = hashPassword(admin.salt, "1");
    const QList<AccountInfo> defaults = {admin};
    if (!save(defaults, errorMessage)) return false;
    accounts_ = defaults;
    return true;
}

bool AccountStore::authenticate(const QString& username, const QString& password, AccountInfo* account) const
{
    const QString normalized = username.trimmed();
    for (const AccountInfo& accountInfo : accounts_) {
        if (accountInfo.username == normalized && accountInfo.enabled &&
            accountInfo.passwordHash == hashPassword(accountInfo.salt, password)) {
            if (account) *account = accountInfo;
            return true;
        }
    }
    return false;
}

bool AccountStore::save(const QList<AccountInfo>& accounts, QString* errorMessage) const
{
    QDomDocument document;
    QDomElement root = document.createElement("accounts");
    root.setAttribute("version", "1");
    document.appendChild(root);
    for (const AccountInfo& account : accounts) {
        QDomElement element = document.createElement("account");
        element.setAttribute("username", account.username);
        element.setAttribute("role", account.role);
        element.setAttribute("enabled", account.enabled ? "true" : "false");
        element.setAttribute("salt", account.salt);
        element.setAttribute("passwordHash", account.passwordHash);
        root.appendChild(element);
    }

    QSaveFile file(path_);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        if (errorMessage) *errorMessage = file.errorString();
        return false;
    }
    QTextStream out(&file);
    document.save(out, 4);
    if (!file.commit()) {
        if (errorMessage) *errorMessage = file.errorString();
        return false;
    }
    return true;
}

bool AccountStore::createUser(const QString& username, const QString& password, QString* errorMessage)
{
    const QString normalized = username.trimmed();
    if (normalized.isEmpty() || password.isEmpty()) {
        if (errorMessage) *errorMessage = "账号和密码不能为空";
        return false;
    }
    for (const AccountInfo& account : accounts_) {
        if (account.username == normalized) {
            if (errorMessage) *errorMessage = "账号已存在";
            return false;
        }
    }

    QList<AccountInfo> candidate = accounts_;
    AccountInfo account;
    account.username = normalized;
    account.role = "user";
    account.salt = QString::number(QRandomGenerator::global()->generate64(), 16);
    account.passwordHash = hashPassword(account.salt, password);
    candidate.append(account);
    if (!save(candidate, errorMessage)) return false;
    accounts_ = candidate;
    return true;
}

bool AccountStore::setEnabled(const QString& username, bool enabled, QString* errorMessage)
{
    QList<AccountInfo> candidate = accounts_;
    for (AccountInfo& account : candidate) {
        if (account.username == username) {
            if (account.role == "admin" && !enabled) {
                if (errorMessage) *errorMessage = "不能停用管理员";
                return false;
            }
            account.enabled = enabled;
            if (!save(candidate, errorMessage)) return false;
            accounts_ = candidate;
            return true;
        }
    }
    if (errorMessage) *errorMessage = "账号不存在";
    return false;
}

bool AccountStore::resetPassword(const QString& username, const QString& password, QString* errorMessage)
{
    if (password.isEmpty()) {
        if (errorMessage) *errorMessage = "密码不能为空";
        return false;
    }

    QList<AccountInfo> candidate = accounts_;
    for (AccountInfo& account : candidate) {
        if (account.username == username) {
            account.salt = QString::number(QRandomGenerator::global()->generate64(), 16);
            account.passwordHash = hashPassword(account.salt, password);
            if (!save(candidate, errorMessage)) return false;
            accounts_ = candidate;
            return true;
        }
    }
    if (errorMessage) *errorMessage = "账号不存在";
    return false;
}

bool AccountStore::deleteUser(const QString& username, QString* errorMessage)
{
    QList<AccountInfo> candidate = accounts_;
    for (int i = 0; i < candidate.size(); ++i) {
        if (candidate[i].username == username) {
            if (candidate[i].role == "admin") {
                if (errorMessage) *errorMessage = "不能删除管理员";
                return false;
            }
            candidate.removeAt(i);
            if (!save(candidate, errorMessage)) return false;
            accounts_ = candidate;
            return true;
        }
    }
    if (errorMessage) *errorMessage = "账号不存在";
    return false;
}
