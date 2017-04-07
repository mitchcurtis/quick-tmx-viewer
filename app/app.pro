TEMPLATE = app

QT += qml quick
CONFIG += c++11

HEADERS += \
    mapview.h \
    tileimageprovider.h

SOURCES += main.cpp \
    mapview.cpp \
    tileimageprovider.cpp

RESOURCES += qml.qrc

DEFINES += QT_DEPRECATED_WARNINGS

LIBS += -L"$$clean_path($$OUT_PWD/../3rdparty/tiled/lib)" -ltiled
INCLUDEPATH += "../3rdparty/tiled/src"

DEFINES += EXAMPLE_DIR=\\\"$$clean_path($$PWD/../3rdparty/tiled/examples)\\\"

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
