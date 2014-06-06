TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../../com
win32{
LIBS += -lws2_32 -lpthread -lregex
}

unix{
LIBS += -lpthread -lrt
}

SOURCES += \
    xs_mem_config.c \
    ../../com/xs_tree.c \
    ../../com/xs_stat.c \
    ../../com/xs_sort.c \
    ../../com/xs_sock.c \
    ../../com/xs_server.c \
    ../../com/xs_rbtree.c \
    ../../com/xs_popen.c \
    ../../com/xs_net.c \
    ../../com/xs_model.c \
    ../../com/xs_mempool.c \
    ../../com/xs_md5.c \
    ../../com/xs_malloc.c \
    ../../com/xs_log.c \
    ../../com/xs_ev.c \
    ../../com/xs_ctrl.c \
    ../../com/xs_config.c \
    ../../com/xs_base64.c \
    ../../com/xs_aio.c \
    ../../com/xs.c \
    ../../example/timer/main.c

HEADERS += \
    ../../com/xs_vec.h \
    ../../com/xs_util.h \
    ../../com/xs_tree.h \
    ../../com/xs_stat.h \
    ../../com/xs_sort.h \
    ../../com/xs_sock.h \
    ../../com/xs_server.h \
    ../../com/xs_rbtree.h \
    ../../com/xs_popen.h \
    ../../com/xs_object.h \
    ../../com/xs_net.h \
    ../../com/xs_model.h \
    ../../com/xs_mempool.h \
    ../../com/xs_md5.h \
    ../../com/xs_malloc.h \
    ../../com/xs_log.h \
    ../../com/xs_list.h \
    ../../com/xs_itoa.h \
    ../../com/xs_heap.h \
    ../../com/xs_hash.h \
    ../../com/xs_ev.h \
    ../../com/xs_dict.h \
    ../../com/xs_def.h \
    ../../com/xs_ctrl.h \
    ../../com/xs_config.h \
    ../../com/xs_base64.h \
    ../../com/xs_atomic.h \
    ../../com/xs_aio.h \
    ../../com/xs.h

