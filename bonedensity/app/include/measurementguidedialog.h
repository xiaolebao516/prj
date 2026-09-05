#pragma once

#include <QDialog>
#include <QString>

class QLabel;
class QPushButton;
class QStackedWidget;
class QWidget;

class MeasurementGuideDialog : public QDialog
{
    Q_OBJECT

public:
    enum class Mode {
        Automatic,
        Manual
    };

    explicit MeasurementGuideDialog(Mode mode, QWidget* parent = nullptr);

    static int currentGuideVersion();
    static bool isCurrentVersionSeen(const QString& settingsPath);
    static bool markCurrentVersionSeen(const QString& settingsPath,
                                       QString* errorMessage = nullptr);

private slots:
    void goBack();
    void goNext();
    void skipOrClose();

private:
    QWidget* createPage(int pageIndex,
                        const QString& heading,
                        const QString& bodyHtml,
                        const QString& emphasis);
    void refreshNavigation();

    Mode mode_;
    QStackedWidget* pages_ = nullptr;
    QLabel* pageIndicator_ = nullptr;
    QPushButton* skipButton_ = nullptr;
    QPushButton* backButton_ = nullptr;
    QPushButton* nextButton_ = nullptr;
};
