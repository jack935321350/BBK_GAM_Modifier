QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TRANSLATIONS += \
    translations/zh_CN.ts

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    colorbutton.cpp \
    g_fun.cpp \
    gam_type.cpp \
    iconview.cpp \
    iconviewedit.cpp \
    iconvieweditdlg.cpp \
    main.cpp \
    mainwidget.cpp

HEADERS += \
    colorbutton.h \
    g_fun.h \
    gam_type.h \
    iconview.h \
    iconviewedit.h \
    iconvieweditdlg.h \
    mainwidget.h

FORMS += \
    iconvieweditdlg.ui \
    mainwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
