# quazip.pri
INCLUDEPATH+= $$PWD/quazip
DEFINES+= QUAZIP_BUILD
LIBS+= -lz

HEADERS+= \
    $$PWD/quazip/crypt.h \
    $$PWD/quazip/ioapi.h \
    $$PWD/quazip/JlCompress.h \
    $$PWD/quazip/quaadler32.h \
    $$PWD/quazip/quachecksum32.h \
    $$PWD/quazip/quacrc32.h\
    $$PWD/quazip/quagzipfile.h \
    $$PWD/quazip/quaziodevice.h \
    $$PWD/quazip/quazipdir.h \
    $$PWD/quazip/quazipfile.h \
    $$PWD/quazip/quazipfileinfo.h\
    $$PWD/quazip/quazip_global.h \
    $$PWD/quazip/quazip.h \
    $$PWD/quazip/quazipnewinfo.h \
    $$PWD/quazip/unzip.h \
    $$PWD/quazip/zip.h \
    $$PWD/zlib/crc32.h \
    $$PWD/zlib/deflate.h \
    $$PWD/zlib/gzguts.h \
    $$PWD/zlib/inffast.h \
    $$PWD/zlib/inffixed.h \
    $$PWD/zlib/inflate.h \
    $$PWD/zlib/inftrees.h \
    $$PWD/zlib/trees.h \
    $$PWD/zlib/zconf.h \
    $$PWD/zlib/zlib.h \
    $$PWD/zlib/zutil.h

SOURCES+= \
    $$PWD/quazip/qioapi.cpp \
    $$PWD/quazip/JlCompress.cpp \
    $$PWD/quazip/quaadler32.cpp \
    $$PWD/quazip/quacrc32.cpp \
    $$PWD/quazip/quagzipfile.cpp \
    $$PWD/quazip/quaziodevice.cpp \
    $$PWD/quazip/quazip.cpp \
    $$PWD/quazip/quazipdir.cpp \
    $$PWD/quazip/quazipfile.cpp\
    $$PWD/quazip/quazipfileinfo.cpp \
    $$PWD/quazip/quazipnewinfo.cpp \
    $$PWD/quazip/unzip.c\
    $$PWD/quazip/zip.c \
    $$PWD/zlib/adler32.c \
    $$PWD/zlib/compress.c \
    $$PWD/zlib/crc32.c \
    $$PWD/zlib/deflate.c \
    $$PWD/zlib/gzclose.c \
    $$PWD/zlib/gzlib.c \
    $$PWD/zlib/gzread.c \
    $$PWD/zlib/gzwrite.c \
    $$PWD/zlib/infback.c \
    $$PWD/zlib/inffast.c \
    $$PWD/zlib/inflate.c \
    $$PWD/zlib/inftrees.c \
    $$PWD/zlib/trees.c \
    $$PWD/zlib/uncompr.c \
    $$PWD/zlib/zutil.c
