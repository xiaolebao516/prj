#pragma once

#include <QList>
#include <QString>

struct AccountInfo {
    QString username;
    QString role;
    bool enabled = true;
    QString salt;
    QString passwordHash;
};

class AccountStore {
public:
    bool loadOrInitialize(const QString& path, QString* errorMessage = nullptr);
    bool authenticate(const QString& username, const QString& password, AccountInfo* account) const;
    const QList<AccountInfo>& accounts() const { return accounts_; }
    bool createUser(const QString& username, const QString& password, QString* errorMessage = nullptr);
    bool setEnabled(const QString& username, bool enabled, QString* errorMessage = nullptr);
    bool resetPassword(const QString& username, const QString& password, QString* errorMessage = nullptr);
    bool deleteUser(const QString& username, QString* errorMessage = nullptr);
private:
    QString path_;
    QList<AccountInfo> accounts_;
    QString hashPassword(const QString& salt, const QString& password) const;
    bool save(QString* errorMessage = nullptr) const;
};
