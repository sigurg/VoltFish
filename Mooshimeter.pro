# NOTICE:
#
# Application name defined in TARGET has a corresponding QML filename.
# If name defined in TARGET is changed, the following needs to be done
# to match new name:
#   - corresponding QML filename must be changed
#   - desktop icon filename must be changed
#   - desktop filename must be changed
#   - icon definition filename in desktop file must be changed
#   - translation filenames have to be changed

# The name of your application
TARGET = Mooshimeter

CONFIG += sailfishapp

SOURCES += src/main.cpp \
    src/qmooshimeter.cpp \
    src/lib/ble.cc \
    src/lib/btcompat.cc \
    src/lib/config_tree.cc \
    src/lib/measurement.cc \
    src/lib/mooshimeter.cc \
    src/lib/reader.cc \
    src/lib/util.cc \
    src/lib/writer.cc

DISTFILES += qml/Mooshimeter.qml \
    qml/cover/CoverPage.qml \
    rpm/Mooshimeter.changes.in \
    rpm/Mooshimeter.changes.run.in \
    rpm/Mooshimeter.spec \
    rpm/Mooshimeter.yaml \
    translations/*.ts \
    Mooshimeter.desktop \
    qml/pages/MainPage.qml \
    qml/pages/Settings.qml \
    qml/cover/util.js \
    qml/lib/ScreenBlank.qml

SAILFISHAPP_ICONS = 86x86 108x108 128x128 256x256

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n

# German translation is enabled as an example. If you aren't
# planning to localize your app, remember to comment out the
# following TRANSLATIONS line. And also do not forget to
# modify the localized app name in the the .desktop file.
TRANSLATIONS += translations/Mooshimeter-de.ts

HEADERS += \
    src/qmooshimeter.h \
    src/lib/ble.h \
    src/lib/config_tree.h \
    src/lib/generic.h \
    src/lib/measurement.h \
    src/lib/mooshimeter.h \
    src/lib/reader.h \
    src/lib/response.h \
    src/lib/util.h \
    src/lib/writer.h

INCLUDEPATH += include/

unix: PKGCONFIG += zlib
