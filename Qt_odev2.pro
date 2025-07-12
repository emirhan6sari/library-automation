QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    kitaplar.cpp \
    main.cpp \
    mainwindow.cpp \
    oduncalma.cpp \
    teslimodunc.cpp \
    uyeler.cpp

HEADERS += \
    kitaplar.h \
    mainwindow.h \
    oduncalma.h \
    teslimodunc.h \
    uyeler.h

FORMS += \
    kitaplar.ui \
    mainwindow.ui \
    oduncalma.ui \
    teslimodunc.ui \
    uyeler.ui

TRANSLATIONS += \
    Qt_odev2_tr_TR.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += resources.qrc \
    images.qrc
