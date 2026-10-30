# BoneDensity desktop application (Qt 6.5.3 / MinGW 11.2, Windows).
# Sources are listed once in src/core/core.pri and src/ui/ui.pri; the test
# projects under tests/ include the same files.
include(version.pri)
include(src/ui/ui.pri)

TARGET = BoneDensity
!isEmpty(BONE_TRIAL_TARGET): TARGET = $$BONE_TRIAL_TARGET

SOURCES += src/app/main.cpp
