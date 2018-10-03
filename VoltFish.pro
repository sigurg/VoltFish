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
TARGET = VoltFish

CONFIG += sailfishapp
CONFIG += c++17

DEFINES += IS_SAILFISH_OS
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

QT += bluetooth

SOURCES += src/main.cpp \
    src/ble-scanner.cpp \
    src/qmooshimeter.cpp \
    src/lib/ble.cc \
    src/lib/btcompat.cc \
    src/lib/config_tree.cc \
    src/lib/measurement.cc \
    src/lib/mooshimeter.cc \
    src/lib/reader.cc \
    src/lib/util.cc \
    src/lib/writer.cc

DISTFILES += \
    qml/cover/CoverPage.qml \
    rpm/VoltFish.changes \
    rpm/VoltFish.spec \
    rpm/VoltFish.yaml \
    translations/*.ts \
    VoltFish.desktop \
    qml/pages/MainPage.qml \
    qml/cover/util.js \
    qml/lib/ScreenBlank.qml \
    qml/pages/Graph.qml \
    qml/VoltFish.qml \
    qml/pages/settings.qml \
    qml/pages/about.qml \
    qml/pages/discovery.qml

SAILFISHAPP_ICONS = 86x86 108x108 128x128 256x256

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n \
    sailfishapp_i18n_idbased

# German translation is enabled as an example. If you aren't
# planning to localize your app, remember to comment out the
# following TRANSLATIONS line. And also do not forget to
# modify the localized app name in the the .desktop file.
TRANSLATIONS += translations/*.ts

HEADERS += \
    src/ble-scanner.h \
    src/qmooshimeter.h \
    src/lib/ble.h \
    src/lib/config_tree.h \
    src/lib/generic.h \
    src/lib/measurement.h \
    src/lib/mooshimeter.h \
    src/lib/reader.h \
    src/lib/response.h \
    src/lib/util.h \
    src/lib/writer.h \
    include/bluetooth/bluetooth.h \
    include/bluetooth/bnep.h \
    include/bluetooth/cmtp.h \
    include/bluetooth/hci.h \
    include/bluetooth/hci_lib.h \
    include/bluetooth/hidp.h \
    include/bluetooth/l2cap.h \
    include/bluetooth/rfcomm.h \
    include/bluetooth/sco.h \
    include/bluetooth/sdp.h \
    include/bluetooth/sdp_lib.h

INCLUDEPATH += include/

unix: PKGCONFIG += zlib
