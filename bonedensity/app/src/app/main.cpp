#include "mainwindow/mainwindow.h"

#include "storage/datalocation.h"
#include "storage/legacyimport.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QLockFile>
#include <QMessageBox>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    const QString dataRoot = DataLocation::root();
    QLockFile instanceLock(QDir(dataRoot).filePath(QStringLiteral("BoneDensity.instance.lock")));
    instanceLock.setStaleLockTime(0);
    if (!instanceLock.tryLock()) {
        QString title;
        QString message;
        switch (instanceLock.error()) {
        case QLockFile::LockFailedError:
            title = QStringLiteral("软件已在运行");
            message = QStringLiteral(
                "同一数据文件夹只能由一个程序使用。\n"
                "请先关闭已经打开的骨密度检测软件，再重试。");
            break;
        case QLockFile::PermissionError:
            title = QStringLiteral("数据文件夹不可写");
            message = QStringLiteral(
                "软件无法在数据文件夹创建运行锁和保存数据：\n%1\n"
                "请将整个软件文件夹复制到桌面或“文档”等可写位置后再运行。")
                          .arg(QDir::toNativeSeparators(dataRoot));
            break;
        case QLockFile::UnknownError:
        default:
            title = QStringLiteral("软件无法启动");
            message = QStringLiteral(
                "无法创建运行锁，请检查数据文件夹所在磁盘是否可用、可写，"
                "然后重试。");
            break;
        }
        QMessageBox::warning(
            nullptr,
            title,
            message);
        return 2;
    }

    // BoneDensity.exe --import-legacy <folder> [...]: import data written by
    // older versions next to a program into the data folder, print the result
    // and exit without opening the window.
    const QStringList args = QCoreApplication::arguments();
    const int importAt = args.indexOf(QStringLiteral("--import-legacy"));
    if (importAt >= 0) {
        QTextStream out(stdout);
        int status = 0;
        for (int i = importAt + 1; i < args.size(); ++i) {
            const LegacyImport::Result result = LegacyImport::importFrom(args.at(i), dataRoot);
            out << "== " << QDir::toNativeSeparators(args.at(i)) << "\n"
                << LegacyImport::summary(result) << "\n";
            if (!result.reportPath.isEmpty())
                out << "report: " << QDir::toNativeSeparators(result.reportPath) << "\n";
            out << "\n";
            if (!result.ok) status = 1;
        }
        out.flush();
        return status;
    }

    // Older versions kept their data next to the program: take it over once.
    const QString programDir = QCoreApplication::applicationDirPath();
    if (LegacyImport::hasLegacyData(programDir)) {
        const LegacyImport::Result result = LegacyImport::importFrom(programDir, dataRoot);
        if (result.performed && (!result.ok || !result.notes.isEmpty())) {
            QMessageBox::warning(
                nullptr, QStringLiteral("旧数据导入"),
                QStringLiteral("已把程序目录中的旧数据导入数据文件夹：\n%1\n\n%2\n\n完整报告：%3")
                    .arg(QDir::toNativeSeparators(dataRoot), LegacyImport::summary(result),
                         QDir::toNativeSeparators(result.reportPath)));
        }
    }

    MainWindow w;
    // The designed size is 1920x1080; maximizing fits it to whatever screen
    // the station has instead of opening partly off-screen on a 1366x768 one.
    w.showMaximized();
    return a.exec();
}
