#include <QtTest>

#include "accountstore.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryDir>

class AccountStoreTests : public QObject
{
    Q_OBJECT

private slots:
    void firstRunCreatesUsableDefaultAdmin();
    void corruptFileIsBackedUpAndRecovered();
    void userLifecyclePersistsAndProtectsAdmin();
    void failedWriteDoesNotChangeInMemoryAccounts();
};

void AccountStoreTests::firstRunCreatesUsableDefaultAdmin()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QString path = directory.filePath(QStringLiteral("accounts.xml"));

    AccountStore store;
    QString error;
    QVERIFY2(store.loadOrInitialize(path, &error), qPrintable(error));
    QVERIFY(QFileInfo::exists(path));
    QCOMPARE(store.accounts().size(), 1);

    AccountInfo account;
    QVERIFY(store.authenticate(QStringLiteral(" admin "), QStringLiteral("1"), &account));
    QCOMPARE(account.username, QStringLiteral("admin"));
    QCOMPARE(account.role, QStringLiteral("admin"));
    QVERIFY(!store.authenticate(QStringLiteral("admin"), QStringLiteral("wrong"), nullptr));

    AccountStore reloaded;
    QVERIFY2(reloaded.loadOrInitialize(path, &error), qPrintable(error));
    QVERIFY(reloaded.authenticate(QStringLiteral("admin"), QStringLiteral("1"), nullptr));
}

void AccountStoreTests::corruptFileIsBackedUpAndRecovered()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QString path = directory.filePath(QStringLiteral("accounts.xml"));

    QFile file(path);
    QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
    QCOMPARE(file.write("<accounts><account"), qint64(18));
    file.close();

    AccountStore store;
    QString error;
    QVERIFY2(store.loadOrInitialize(path, &error), qPrintable(error));
    QVERIFY(store.authenticate(QStringLiteral("admin"), QStringLiteral("1"), nullptr));
    QVERIFY(QFileInfo::exists(path + QStringLiteral(".bad.bak")));

    AccountStore reloaded;
    QVERIFY2(reloaded.loadOrInitialize(path, &error), qPrintable(error));
    QVERIFY(reloaded.authenticate(QStringLiteral("admin"), QStringLiteral("1"), nullptr));
}

void AccountStoreTests::userLifecyclePersistsAndProtectsAdmin()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QString path = directory.filePath(QStringLiteral("accounts.xml"));

    AccountStore store;
    QString error;
    QVERIFY2(store.loadOrInitialize(path, &error), qPrintable(error));
    QVERIFY2(store.createUser(QStringLiteral(" 护士&甲 "), QStringLiteral("初始密码"), &error),
             qPrintable(error));
    QVERIFY(store.authenticate(QStringLiteral("护士&甲"), QStringLiteral("初始密码"), nullptr));
    QVERIFY(!store.createUser(QStringLiteral("护士&甲"), QStringLiteral("other"), &error));

    QVERIFY2(store.setEnabled(QStringLiteral("护士&甲"), false, &error), qPrintable(error));
    QVERIFY(!store.authenticate(QStringLiteral("护士&甲"), QStringLiteral("初始密码"), nullptr));
    QVERIFY2(store.setEnabled(QStringLiteral("护士&甲"), true, &error), qPrintable(error));
    QVERIFY2(store.resetPassword(QStringLiteral("护士&甲"), QStringLiteral("新密码"), &error),
             qPrintable(error));
    QVERIFY(!store.authenticate(QStringLiteral("护士&甲"), QStringLiteral("初始密码"), nullptr));
    QVERIFY(store.authenticate(QStringLiteral("护士&甲"), QStringLiteral("新密码"), nullptr));

    QVERIFY(!store.setEnabled(QStringLiteral("admin"), false, &error));
    QVERIFY(!store.deleteUser(QStringLiteral("admin"), &error));
    QVERIFY(store.authenticate(QStringLiteral("admin"), QStringLiteral("1"), nullptr));

    AccountStore reloaded;
    QVERIFY2(reloaded.loadOrInitialize(path, &error), qPrintable(error));
    QVERIFY(reloaded.authenticate(QStringLiteral("护士&甲"), QStringLiteral("新密码"), nullptr));
    QVERIFY2(reloaded.deleteUser(QStringLiteral("护士&甲"), &error), qPrintable(error));

    AccountStore afterDelete;
    QVERIFY2(afterDelete.loadOrInitialize(path, &error), qPrintable(error));
    QVERIFY(!afterDelete.authenticate(QStringLiteral("护士&甲"), QStringLiteral("新密码"), nullptr));
    QVERIFY(afterDelete.authenticate(QStringLiteral("admin"), QStringLiteral("1"), nullptr));
}

void AccountStoreTests::failedWriteDoesNotChangeInMemoryAccounts()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QString path = directory.filePath(QStringLiteral("accounts.xml"));

    AccountStore store;
    QString error;
    QVERIFY2(store.loadOrInitialize(path, &error), qPrintable(error));
    QVERIFY2(store.createUser(QStringLiteral("operator"), QStringLiteral("old"), &error),
             qPrintable(error));

    QVERIFY(QFile::remove(path));
    QVERIFY(QDir().mkdir(path));

    QVERIFY(!store.resetPassword(QStringLiteral("operator"), QStringLiteral("new"), &error));
    QVERIFY(!error.isEmpty());
    QVERIFY(store.authenticate(QStringLiteral("operator"), QStringLiteral("old"), nullptr));
    QVERIFY(!store.authenticate(QStringLiteral("operator"), QStringLiteral("new"), nullptr));

    QVERIFY(!store.setEnabled(QStringLiteral("operator"), false, &error));
    QVERIFY(store.authenticate(QStringLiteral("operator"), QStringLiteral("old"), nullptr));
    QVERIFY(!store.createUser(QStringLiteral("other"), QStringLiteral("password"), &error));
    QCOMPARE(store.accounts().size(), 2);
    QVERIFY(!store.deleteUser(QStringLiteral("operator"), &error));
    QVERIFY(store.authenticate(QStringLiteral("operator"), QStringLiteral("old"), nullptr));
}

QTEST_MAIN(AccountStoreTests)
#include "accountstore_tests.moc"
