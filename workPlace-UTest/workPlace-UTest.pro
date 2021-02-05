QT += testlib
QT += gui
CONFIG += qt warn_on depend_includepath testcase

TEMPLATE = app

SOURCES +=  tst_tworkplace.cpp \
    ../workPlace-master/person.cpp \
    ../workPlace-master/task.cpp \
    ../workPlace-master/tasktable.cpp \
    ../workPlace-master/user.cpp \
    ../workPlace-master/worker.cpp \
    ../workPlace-master/workerlist.cpp

HEADERS += \
    ../workPlace-master/person.h \
    ../workPlace-master/task.h \
    ../workPlace-master/tasktable.h \
    ../workPlace-master/user.h \
    ../workPlace-master/worker.h \
    ../workPlace-master/workerlist.h
