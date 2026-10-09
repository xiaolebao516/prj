# Core: device protocol, measurement pipeline, calibration, storage and
# bone-health assessment. Qt Core/Xml only: core code must never include a
# Gui/Widgets header (the core-only test builds enforce this).
QT += core xml
CONFIG += c++17
INCLUDEPATH += $$PWD

# Isolated research trial builds, at most one and Debug only, selected with
# CONFIG+=<name>. Only the measurement profile reads the BONE_*_EXPERIMENT
# define; BONE_TRIAL_TARGET names the separate executable.
BONE_TRIALS = observe_before_g_trial dual_window_a_trial b_peak_completion_trial relock_preservation_trial
observe_before_g_trial.define = BONE_OBSERVE_BEFORE_G_EXPERIMENT
observe_before_g_trial.target = BoneDensity_SelfTrial
dual_window_a_trial.define = BONE_DUAL_WINDOW_A_EXPERIMENT
dual_window_a_trial.target = BoneDensity_DualWindowTrial
b_peak_completion_trial.define = BONE_COMPLETE_B_PEAK_EXPERIMENT
b_peak_completion_trial.target = BoneDensity_BPeakTrial
relock_preservation_trial.define = BONE_RELOCK_PRESERVATION_EXPERIMENT
relock_preservation_trial.target = BoneDensity_RelockTrial
for(trial, BONE_TRIALS) {
    contains(CONFIG, $$trial) {
        !isEmpty(BONE_TRIAL): error("Select only one trial profile")
        BONE_TRIAL = $$trial
    }
}
!isEmpty(BONE_TRIAL) {
    !CONFIG(debug, debug|release): error("The $$BONE_TRIAL profile must be a Debug build")
    DEFINES += $$eval($${BONE_TRIAL}.define)
    BONE_TRIAL_TARGET = $$eval($${BONE_TRIAL}.target)
}

HEADERS += \
    $$PWD/records.h \
    $$PWD/device/deviceprotocol.h \
    $$PWD/measurement/measurementtypes.h \
    $$PWD/measurement/measurementprofile.h \
    $$PWD/measurement/signalprocessor.h \
    $$PWD/measurement/frameanalyzer.h \
    $$PWD/measurement/measurementsession.h \
    $$PWD/measurement/utils.h \
    $$PWD/measurement/parametergroup.h \
    $$PWD/measurement/measurementexperimentlog.h \
    $$PWD/calibration/calibration.h \
    $$PWD/calibration/calibrationstore.h \
    $$PWD/storage/accountstore.h \
    $$PWD/storage/patientstore.h \
    $$PWD/storage/databackup.h \
    $$PWD/storage/datalocation.h \
    $$PWD/storage/legacyimport.h \
    $$PWD/health/sosreference.h \
    $$PWD/health/bonehealth.h

SOURCES += \
    $$PWD/device/deviceprotocol.cpp \
    $$PWD/measurement/measurementprofile.cpp \
    $$PWD/measurement/signalprocessor.cpp \
    $$PWD/measurement/frameanalyzer.cpp \
    $$PWD/measurement/measurementsession.cpp \
    $$PWD/measurement/utils.cpp \
    $$PWD/measurement/parametergroup.cpp \
    $$PWD/calibration/calibration.cpp \
    $$PWD/calibration/calibrationstore.cpp \
    $$PWD/storage/accountstore.cpp \
    $$PWD/storage/patientstore.cpp \
    $$PWD/storage/databackup.cpp \
    $$PWD/storage/datalocation.cpp \
    $$PWD/storage/legacyimport.cpp \
    $$PWD/health/sosreference.cpp \
    $$PWD/health/bonehealth.cpp
