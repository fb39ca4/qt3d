TARGET = tank
TEMPLATE = app
CONFIG += qt warn_on
!package: CONFIG += qt3d

SOURCES += main.cpp\
    tankview.cpp \
    tank.cpp \
    quadplane.cpp

HEADERS  += tankview.h \
    tank.h \
    quadplane.h

include(../../../qt3d_pkg_dep.pri)

OTHER_FILES += \
    tank.rc

RC_FILE = tank.rc
