
/*
 * This file is part of the xs Library.
 *
 * Copyright (C) 2011 by Guoliang Xue <xueguoliang@gmail.com>
 *
 * The xs Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The xs Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the xs Library. If not, see <http://www.gnu.org/licenses/>.
 */
#ifdef __cplusplus
extern "C"{
#endif
#ifndef __XSEV_H__
#define __XSEV_H__

#define XS_EV_NONE 0
#define XS_EV_FREE 1
#define XS_EV_SOCK 2
#define XS_EV_TIME 3
#define XS_EV_THRE 4
#define XS_EV_QUIT 5 
#define XS_EV_ADD_THRE 6
#define XS_EV_KILL_THRE 7
#define XS_EV_MSG 8

#define XS_EV_TRANS_ALL 0

#define XS_MAX_THRE 4096

typedef struct xs_ev_t
{
    xs_list_t   node;
    int         type;
    
    char        buf[0];
} xs_ev_t;

#define XS_EV_INIT(__ev, __type) do { (__ev)->type = __type; xs_list_init(&(__ev)->node);} while(0)

typedef struct xs_ev_sock_t
{
    int         fd;
    int         event; /* EPOLLIN or EPOLLOUT */
    void        (*func)(struct xs_ev_sock_t*);
    void*       arg;
    xs_ev_t*    ev;
} xs_ev_sock_t;

typedef struct xs_ev_time_t
{
    pthread_mutex_t mutex;
    int     cancel;
    int     period;
    uint64_t    expire;
    void        (*func)(struct xs_ev_time_t*);
    void*       arg;
    xs_ev_t*    ev;
} xs_ev_time_t;

typedef struct xs_ev_thre_t
{
    int         thid;
    int         fd;
    xs_ev_t*    ev;
} xs_ev_thre_t;

typedef struct xs_ev_msg_t
{
#define XS_EV_MSG_MEM_CHECK 0
#define XS_EV_MSG_INIT 1 /* the function need alloc global memory, can not do in sub thread,just in main thread */
    int         type;
    int         thid; /* send to the thread id, 0 mean main thread */
    xs_ev_t*    ev;
    void*       arg;
    void        (*func)(struct xs_ev_msg_t*);
} xs_ev_msg_t;

#define XS_TIME_EV(__ev) ((xs_ev_time_t*)(__ev)->buf)
#define XS_THRE_EV(__ev) ((xs_ev_thre_t*)(__ev)->buf)
#define XS_SOCK_EV(__ev) ((xs_ev_sock_t*)(__ev)->buf)
#define XS_MSG_EV(__ev) ((xs_ev_msg_t *)(__ev)->buf)

typedef struct xs_thread_t
{
    xs_mempool_t*   mp;

    xs_list_t       node;
    int             quit;
    XSOCKET             fd;

    xs_list_t       ev_que;
    xs_list_t       ev_recv;
    xs_list_t       ev_send;

    xs_ev_thre_t*    thev;
} xs_thread_t;

typedef struct xs_process_t
{
    xs_mempool_t*   mp;
    void(*_quit)();
    xs_list_t       ev_recv1;

    xs_list_t       thre_stop;
    xs_thread_t*    thre[XS_MAX_THRE]; /* position 0 is not used */
    int             epoll_fd;
    int16_t         thre_count;
    int16_t         quit;
    xs_heap_t*      timer;
#if XS_EV_TRANS_ALL
#else
    int             ev_count;
#endif
} xs_process_t;

extern xs_process_t g_process;

int xs_ev_init(int thre_count, void(*quit)());
int xs_ev_run();

int xs_ev_quit();

/* is quiting */
#define xs_ev_quiting() g_process.quit

int xs_ev_add_thre();
int xs_ev_kill_thre();

int xs_ev_get_thre_count();

int xs_ev_add1(xs_ev_t* ev, const char* file, int line);
#define xs_ev_add(ev) xs_ev_add1(ev, __FILE__, __LINE__)

xs_ev_sock_t* xs_ev_add_sock_ev1(XSOCKET fd, int event, void(*func)(xs_ev_sock_t*), void* ptr, const char* file, int line);
xs_ev_time_t* xs_ev_add_time_ev(int period, void(*func)(xs_ev_time_t*), void* ptr);
xs_ev_thre_t* xs_ev_add_thre_ev(XSOCKET fd, int thid);
xs_ev_msg_t* xs_ev_add_msg_ev1(int type, int thid, void(*func)(xs_ev_msg_t*), void* ptr, const char* file, int line);
#define xs_ev_add_msg_ev(__type, __thid, __func, __ptr) \
    xs_ev_add_msg_ev1(__type, __thid, __func, __ptr, __FILE__, __LINE__)

void xs_ev_time_restart(xs_ev_time_t* ev, int period);

static inline void xs_ev_kill_time(xs_ev_time_t* ev)
{
    pthread_mutex_lock(&ev->mutex);
    ev->cancel = 1;
    ev->arg = NULL;
    pthread_mutex_unlock(&ev->mutex);
}

static inline void xs_ev_time_destroy(xs_ev_time_t* ev)
{
    pthread_mutex_destroy(&ev->mutex);
    xs_free(ev->ev);
}
#define xs_timer_lock(ev) pthread_mutex_lock(&ev->mutex)
#define xs_timer_unlock(ev) pthread_mutex_unlock(&ev->mutex)
#define xs_quiting() g_process.quit

xs_ev_time_t* xs_new_time_ev(int period, void(*func)(xs_ev_time_t*), void* arg);
xs_ev_thre_t* xs_new_thre_ev(int id, XSOCKET fd);
xs_ev_sock_t* xs_new_sock_ev1(XSOCKET fd, int event, void(*func)(xs_ev_sock_t*), void* ptr, const char* file, int line);
xs_ev_msg_t* xs_new_msg_ev1(int type, int thid, void(*func)(xs_ev_msg_t*), void* ptr, const char* file, int line);

#define xs_new_msg_ev(__type, __thid, __func, __ptr) xs_new_msg_ev1(__type, __thid, __func, __ptr, __FILE__, __LINE__)
#define xs_new_sock_ev(__fd, __event, __func, __ptr) xs_new_sock_ev1(__fd, __event, __func, __ptr, __FILE__, __LINE__)
#define xs_ev_add_sock_ev(__fd, __event, __func, __ptr) xs_ev_add_sock_ev1(__fd, __event, __func, __ptr, __FILE__, __LINE__)

#endif
#ifdef __cplusplus
}
#endif
