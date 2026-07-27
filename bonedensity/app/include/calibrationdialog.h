#pragma once

#include "calibration.h"
#include "calibrationstore.h"

#include <QDialog>

class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QLabel;
class QLineEdit;
class QProgressBar;
class QPushButton;
class QStackedWidget;
class QTableWidget;
class CalibrationTests;

class CalibrationDialog : public QDialog
{
    Q_OBJECT

public:
    CalibrationDialog(CalibrationStore* store,
                      const QString& operatorName,
                      QWidget* parent = nullptr);

    void submitFrame(const CalibrationFrame& frame);
    void notifyAcquisitionUnavailable(const QString& reason);

signals:
    void acquisitionStartRequested(double processingD);
    void acquisitionStopRequested();
    void activeDChanged(double activeD);

protected:
    void closeEvent(QCloseEvent* event) override;
    void reject() override;

private slots:
    void goBack();
    void goNext();
    void startCalibrationMeasurement();
    void startValidationMeasurement();
    void activateCandidate();
    void restoreDefault();
    void restorePrevious();
    void showHistory();
    void updateModeFields();

private:
    friend class CalibrationTests;

    QWidget* createIntroductionPage();
    QWidget* createSetupPage();
    QWidget* createCollectionPage(CalibrationPhase phase);
    QWidget* createRepositionPage();
    QWidget* createResultPage();
    void configureSessionAndContinue();
    void startMeasurement(CalibrationPhase phase);
    void finishAcquisitionUi(CalibrationPhase phase);
    void refreshCurrentParameters();
    void refreshMeasurementTables();
    void refreshNavigation();
    void refreshResult();
    bool saveCurrentRecord(bool activate);
    bool prepareToClose();
    QTableWidget* tableForPhase(CalibrationPhase phase) const;
    QProgressBar* progressForPhase(CalibrationPhase phase) const;
    QLabel* statusForPhase(CalibrationPhase phase) const;
    QPushButton* startButtonForPhase(CalibrationPhase phase) const;
    QCheckBox* operationCheckForPhase(CalibrationPhase phase) const;
    QDoubleSpinBox* temperatureForPhase(CalibrationPhase phase) const;

    CalibrationStore* store_ = nullptr;
    CalibrationSession session_;
    bool recordSaved_ = false;
    bool activated_ = false;
    bool closeApproved_ = false;

    QLabel* stepLabel_ = nullptr;
    QStackedWidget* pages_ = nullptr;
    QPushButton* backButton_ = nullptr;
    QPushButton* nextButton_ = nullptr;
    QPushButton* closeButton_ = nullptr;

    QLabel* currentDLabel_ = nullptr;
    QLabel* previousDLabel_ = nullptr;
    QLabel* defaultDLabel_ = nullptr;

    QComboBox* modeCombo_ = nullptr;
    QLineEdit* operatorEdit_ = nullptr;
    QLineEdit* probeIdEdit_ = nullptr;
    QLineEdit* phantomModelEdit_ = nullptr;
    QLineEdit* phantomSerialEdit_ = nullptr;
    QLineEdit* certificateEdit_ = nullptr;
    QDoubleSpinBox* referenceTemperatureSpin_ = nullptr;
    QDoubleSpinBox* referenceSosSpin_ = nullptr;
    QCheckBox* certificateConfirmedCheck_ = nullptr;
    QCheckBox* responsibilityCheck_ = nullptr;
    QLabel* setupModeHint_ = nullptr;

    QDoubleSpinBox* calibrationTemperatureSpin_ = nullptr;
    QDoubleSpinBox* validationTemperatureSpin_ = nullptr;
    QCheckBox* calibrationOperationCheck_ = nullptr;
    QCheckBox* validationOperationCheck_ = nullptr;
    QProgressBar* calibrationProgress_ = nullptr;
    QProgressBar* validationProgress_ = nullptr;
    QTableWidget* calibrationTable_ = nullptr;
    QTableWidget* validationTable_ = nullptr;
    QLabel* calibrationStatus_ = nullptr;
    QLabel* validationStatus_ = nullptr;
    QLabel* candidateLabel_ = nullptr;
    QPushButton* calibrationStartButton_ = nullptr;
    QPushButton* validationStartButton_ = nullptr;

    QCheckBox* repositionConfirmedCheck_ = nullptr;
    QLabel* resultLabel_ = nullptr;
    QPushButton* activateButton_ = nullptr;
};
