isEmpty(ZLIB_PATH):ZLIB_PATH = ../../../zlib


INCLUDEPATH += $${ZLIB_PATH}


SOURCES += \
    $${ZLIB_PATH}/zutil.c \
    $${ZLIB_PATH}/uncompr.c \
    $${ZLIB_PATH}/trees.c \
    $${ZLIB_PATH}/inftrees.c \
    $${ZLIB_PATH}/inflate.c \
    $${ZLIB_PATH}/inffast.c \
    $${ZLIB_PATH}/deflate.c \
    $${ZLIB_PATH}/crc32.c \
    $${ZLIB_PATH}/compress.c \
    $${ZLIB_PATH}/adler32.c \
    $${ZLIB_PATH}/gzlib.c \
    $${ZLIB_PATH}/gzread.c \
