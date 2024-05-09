QT       += core gui
QT       += multimedia
QT       += multimediawidgets
QT       += multimedia network
QT       += widgets
QT       += sql
QT       += texttospeech
CONFIG   += console
CONFIG   += debug
RC_ICONS = $$PWD/picture/LOGO.ico
DESTDIR = $$PWD/bin
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    audio.cpp \
    changecolorform.cpp \
    http.cpp \
    lyricwidget.cpp \
    main.cpp \
    speech.cpp \
    widget.cpp\
    localmusic.cpp\
    dbManager.cpp

HEADERS += \
    audio.h \
    changecolorform.h \
    http.h \
    loadfileqss.h \
    lyricwidget.h \
    speech.h \
    widget.h\
    localmusic.h\
    dbManager.h

FORMS += \
    changecolorform.ui \
    widget.ui\

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    picture.qrc
