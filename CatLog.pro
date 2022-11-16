TEMPLATE = lib
# DEFINES += UNTITLED1_LIBRARY

CONFIG += c++11

include($$PWD/CatLog/CatLog.pri)

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
