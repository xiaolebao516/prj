#include "mainwindow.h"

#include <QApplication>
#include <QCoreApplication>
#include <QLockFile>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QLockFile instanceLock(
        QCoreApplication::applicationDirPath() + QStringLiteral("/BoneDensity.instance.lock"));
    instanceLock.setStaleLockTime(0);
    if (!instanceLock.tryLock()) {
        QString title;
        QString message;
        switch (instanceLock.error()) {
        case QLockFile::LockFailedError:
            title = QStringLiteral("软件已在运行");
            message = QStringLiteral(
                "同一软件文件夹只能运行一个程序。\n"
                "请先关闭已经打开的骨密度检测软件，再重试。");
            break;
        case QLockFile::PermissionError:
            title = QStringLiteral("软件文件夹不可写");
            message = QStringLiteral(
                "软件无法在当前文件夹创建运行锁和保存数据。\n"
                "请将整个软件文件夹复制到桌面或“文档”等可写位置后再运行。");
            break;
        case QLockFile::UnknownError:
        default:
            title = QStringLiteral("软件无法启动");
            message = QStringLiteral(
                "无法创建运行锁，请检查软件所在磁盘和文件夹是否可用、可写，"
                "然后重试。");
            break;
        }
        QMessageBox::warning(
            nullptr,
            title,
            message);
        return 2;
    }
    MainWindow w;
    w.show();
    return a.exec();
}
