#pragma once

#include "types.h"

#include <QDate>
#include <QDialog>
#include <QSet>

class QButtonGroup;
class QLabel;
class QLineEdit;
class QPushButton;

// One dialog for creating and editing a patient archive entry.
// Gender and birth date have no defaults so an unedited form cannot be saved.
class PatientFormDialog : public QDialog
{
    Q_OBJECT
    friend class MainWindowSafetyTests;

public:
    enum class Mode {
        Create,
        Edit
    };

    explicit PatientFormDialog(Mode mode, QWidget* parent = nullptr);

    void setPatient(const PatientInfo& patient);
    void setSuggestedId(const QString& id);
    void setExistingIds(const QSet<QString>& ids);
    void setSaveButtonText(const QString& text);

    PatientInfo patient() const;
    bool validate();

    static int ageOn(const QDate& birthDate, const QDate& onDate);
    static QString suggestId(const QList<PatientInfo>& patients, const QDate& today);

public slots:
    void accept() override;

private:
    QDate enteredBirthDate() const;
    void updateAgePreview();
    static QLabel* makeErrorLabel(QWidget* parent);

    Mode mode_;
    QSet<QString> existingIds_;
    PatientInfo original_;

    QLineEdit* nameEdit = nullptr;
    QLineEdit* idEdit = nullptr;
    QPushButton* maleButton = nullptr;
    QPushButton* femaleButton = nullptr;
    QButtonGroup* genderGroup = nullptr;
    QLineEdit* yearEdit = nullptr;
    QLineEdit* monthEdit = nullptr;
    QLineEdit* dayEdit = nullptr;
    QLabel* ageLabel = nullptr;
    QLabel* ageHint = nullptr;
    QLineEdit* heightEdit = nullptr;
    QLineEdit* weightEdit = nullptr;
    QLabel* nameError = nullptr;
    QLabel* idError = nullptr;
    QLabel* sexError = nullptr;
    QLabel* bodyError = nullptr;
    QPushButton* saveButton = nullptr;
    QPushButton* cancelButton = nullptr;
};
