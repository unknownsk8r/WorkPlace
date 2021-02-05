QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addworkerwindow.cpp \
    authwindow.cpp \
    main.cpp \
    admin.cpp \
    person.cpp \
    task.cpp \
    tasktable.cpp \
    taskwindow.cpp \
    user.cpp \
    worker.cpp \
    workerlist.cpp

HEADERS += \
    addworkerwindow.h \
    admin.h \
    authwindow.h \
    config.h \
    person.h \
    task.h \
    tasktable.h \
    taskwindow.h \
    user.h \
    worker.h \
    workerlist.h

FORMS += \
    addworkerwindow.ui \
    admin.ui \
    authwindow.ui \
    taskwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
