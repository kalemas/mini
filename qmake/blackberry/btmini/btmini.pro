
# probably should be replaced with "qnx"
CONFIG += blackberry
# probably should be replaced with "Q_OS_QNX"
DEFINES += Q_OS_BLACKBERRY

include(../../common/btmini/btmini.pro)

OTHER_FILES += \
    bar-descriptor.xml
