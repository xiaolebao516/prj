# Core-only: no Gui/Widgets on the include path, so the whole core must build without them.
QT = core testlib
include(tests.pri)
include(../src/core/core.pri)

TARGET = accountstore_tests
SOURCES += accountstore_tests.cpp
