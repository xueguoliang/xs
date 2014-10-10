
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
#include "xs.h"

#define __xs_wakeup(th) do{\
    char __buf = 1;\
    send((th)->thev->fd, &__buf, 1, 0);\
}while(0)

xs_process_t    g_process;

static inline void __xs_ev_trans(xs_list_t* to, xs_list_t* from)
{
#if XS_EV_TRANS_ALL
    xs_list_append(to, from);
#else
    xs_list_move_tail(to, from->next);
    -- g_process.ev_count;
#endif
}

static inline void __xs_ev_recv(xs_list_t* to, xs_list_t* node)
{
#if XS_EV_TRANS_ALL
#else
    ++g_process.ev_count;
#endif
    xs_list_add_tail(to, node);
}

uint64_t __xs_timer_now()
{
    uint64_t now;
#ifdef WIN32

    LARGE_INTEGER t;
#if 0
    static ULONGLONG feq1 = -1;
    if(feq1==(ULONGLONG)-1)
    {
        LARGE_INTEGER feq;
        if(!QueryPerformanceFrequency(&feq))
        {
            xs_loge("get frequency error\n");
            exit(1);
        }
        feq1 = feq.QuadPart;
    }
    if(!QueryPerformanceCounter(&t))
    {
        xs_loge("get counter error\n");
        exit(1);
    }
    now = t.QuadPart*1000/feq1;
    xs_logd("now is %I64u", now);
#else
    SYSTEMTIME st;
    GetSystemTime(&st);
    FILETIME ft;
    SystemTimeToFileTime(&st, &ft);
    t.u.HighPart = ft.dwHighDateTime;
    t.u.LowPart = ft.dwLowDateTime;
    now = t.QuadPart/10000;
    xs_logd("now is %I64u", now);

#endif
#else
    struct timespec ts;
    //clock_gettime(CLOCK_MONOTONIC, &ts);
    clock_gettime(CLOCK_REALTIME, &ts);
    
    now = ts.tv_sec * 1000;
    now += ts.tv_nsec / 1000000;
#endif
    return now;
}

xs_ev_time_t* xs_new_time_ev(int period, void(*func)(xs_ev_time_t*), void* arg)
{
    xs_ev_t* ev = xs_malloc(sizeof(xs_ev_t) + sizeof(xs_ev_time_t));
    xs_ev_time_t* t = XS_TIME_EV(ev);

    XS_EV_INIT(ev, XS_EV_TIME);

    t->ev = ev;
    pthread_mutex_init(&t->mutex, NULL);

    t->arg = arg;
    t->cancel = 0;
    t->period = period;
    t->expire = __xs_timer_now() + period;
    t->func = func;
    return t;
}

xs_ev_msg_t* xs_new_msg_ev1(int type, int thid, void(*func)(xs_ev_msg_t*), void* ptr, const char* file, int line)
{
    xs_ev_t* ev = __xs_malloc(sizeof(xs_ev_t) + sizeof(xs_ev_msg_t), file, line);
    xs_ev_msg_t* mev = XS_MSG_EV(ev);
    XS_EV_INIT(ev, XS_EV_MSG);

    mev->ev = ev;
    mev->type = type;
    mev->thid = thid;
    mev->func = func;
    mev->arg = ptr;
    return mev;
}

xs_ev_sock_t* xs_new_sock_ev1(XSOCKET fd, int event, void(*func)(xs_ev_sock_t*), void* ptr, const char* file, int line)
{
    xs_ev_t* ev = __xs_malloc(sizeof(xs_ev_t) + sizeof(xs_ev_sock_t), file, line);
    xs_ev_sock_t* sock = XS_SOCK_EV(ev);
 //   xs_ev_sock_t* sock = (xs_ev_sock_t*)(ev->buf);

    // 初始化xs_ev_t部分
    XS_EV_INIT(ev, XS_EV_SOCK);

    // 初始化具体消息部分
    sock->ev = ev;

    sock->fd = fd;
    sock->event = event;
    sock->func = func;
    sock->arg = ptr;

    return sock;
}

xs_ev_thre_t* xs_new_thre_ev(int id, XSOCKET fd)
{
    xs_ev_t* ev = xs_malloc(sizeof(xs_ev_t) + sizeof(xs_ev_thre_t));
    xs_ev_thre_t* thre = XS_THRE_EV(ev);

    XS_EV_INIT(ev, XS_EV_THRE);

    thre->ev = ev;

    thre->fd = fd;
    thre->thid = id;

    return thre;
}

void __xs_thre_init(xs_thread_t* th, int id)
{
    XSOCKET fd[2];
    th->mp = xs_mempool_create(th, id, 512000);
    xs_list_init(&th->node);// 线程链表结点，用于主线程中的thre_stop
    th->quit = 0; // 线程的退出标记
    th->fd = 0;  // 用于工作线程和主线程通信的socket
    xs_list_init(&th->ev_que);  // 工作线程的工作队列
    xs_list_init(&th->ev_recv); // 工作线程的接收任务队列
    xs_list_init(&th->ev_send); // 工作线程的发送任务队列
#ifdef WIN32
    if(socketpair(AF_INET, SOCK_STREAM, 0, fd) < 0)
#else
    if(socketpair(PF_UNIX, SOCK_STREAM, 0, fd) < 0)
#endif
    {
        xs_loge("socket pair error");
        exit(1);
    }
    th->fd = fd[0];

    th->thev = xs_new_thre_ev(id, fd[1]);
    xs_ev_add(th->thev->ev);// 把线程ev加入到epoll
}

void __xs_thre_wait(xs_thread_t* th)
{
    char buf = 1;
    // 发送一个字节的内容，通知主线程，该工作线程没事儿干了
    send(th->fd, &buf, 1, 0);
    // 阻塞等待主线程的命令
    recv(th->fd, &buf, 1, 0);
}

void __xs_thre_work(xs_thread_t* th)
{
    xs_ev_t* ev;
    xs_ev_time_t* t;
    xs_ev_sock_t* s;
    xs_ev_msg_t* m;
    while(!xs_list_empty(&th->ev_que))
    {
        ev = xs_list_first_entry(&th->ev_que, xs_ev_t, node);
        xs_list_del(&ev->node);
        switch (ev->type)
        {
            case XS_EV_FREE:
                xs_free(ev);
                break;
            case XS_EV_TIME:
                t = XS_TIME_EV(ev);
                pthread_mutex_lock(&t->mutex);
                t->func(t);
                pthread_mutex_unlock(&t->mutex);
                t->expire = __xs_timer_now() + t->period;
                xs_ev_add(t->ev);
#ifdef WIN32
                xs_logd("add time %I64d", t->expire);
#else
                xs_logd("add time %llu", (unsigned long long int)t->expire);
#endif
                break;
            case XS_EV_SOCK:
                s = XS_SOCK_EV(ev);
                xs_logd("thre handle socket ev fd=%d", s->fd);
                s->func(s);
                break;
            case XS_EV_MSG:
                m = XS_MSG_EV(ev);
                if(m->type == XS_EV_MSG_MEM_CHECK)
                {
#ifdef XS_USE_MEMPOOL
                    xs_mempool_check_mem_leak(th->mp);
#endif
                    xs_free(m->ev);
                }
                else
                {
                    m->func(m);
                }
                break;
            default:
                xs_loge("type error=%d", ev->type);
                exit(100);
                break;
        }
    }
}

static inline void __xs_thre_exit(xs_thread_t* th)
{
    char buf = -1;
    send(th->fd, &buf, 1, 0);
}

void* __xs_thre_run(void* ptr)
{
    xs_thread_t* th = (xs_thread_t*)ptr;
    pthread_setspecific(g_thread_key, th->mp);

    while(1)
    {
        /* clear all memory and quit flag is 1, break */
#ifdef XS_USE_MEMPOOL
        if(th->quit && th->mp->use_buff == 0
                && xs_list_empty(&th->ev_que))
#else
        if(th->quit && xs_list_empty(&th->ev_que))
#endif
        {
            break;
        }

        __xs_thre_wait(th);
        __xs_thre_work(th);
    }

    xs_logd("thread %d quit", th->mp->rtid);
    __xs_thre_exit(th);
    return NULL;
}

static inline void __xs_thre_start(xs_thread_t* th)
{
    pthread_t self;
    pthread_create(&self, 0, __xs_thre_run, th);
    pthread_detach(self);
}

int __xs_ev_add_thre()
{
    int i; 
    xs_thread_t* th;
    for(i=1; i<XS_MAX_THRE; i++)
    {
        if(g_process.thre[i] == NULL)
        {
            th = xs_malloc(sizeof(*th));
            g_process.thre[i] = th;
            __xs_thre_init(th, i);
            __xs_thre_start(th);
            ++g_process.thre_count;
            break;
        }
    }
    
    return 0;
}

int __xs_ev_kill_thre()
{
    int i;
    /* it will be quit in sometime */
    xs_thread_t* th;
    for(i=1; i<XS_MAX_THRE; ++i)
    {
        th = g_process.thre[i];
        if(th && th->quit == 0)
        {
            g_process.thre[i]->quit = 1;
            /* it is stopped */
            if(th->node.next != &th->node)
            {
                xs_list_append(&th->ev_que, &th->ev_recv);
                __xs_wakeup(th); 
            }
            break;
        }
    }
    return 0;
}

int __xs_timer_cmp(const void* p1, const void* p2)
{
    const xs_ev_time_t* t1 = (const xs_ev_time_t*)(p1);
    const xs_ev_time_t* t2 = (const xs_ev_time_t*)(p2);

    return t1->expire - t2->expire; 
}

int __xs_timer_check()
{
    xs_heap_t* hp = g_process.timer;
    xs_ev_time_t* time_ev;
    // 当前时间
    uint64_t now = __xs_timer_now();

    while(!xs_heap_empty(hp))
    {
        time_ev = (xs_ev_time_t*)hp->data[0];
        if(time_ev->expire <= now || time_ev->cancel)
        {
            __xs_ev_recv(&g_process.ev_recv1, &time_ev->ev->node);
            xs_heap_del(hp, 0);
        }
        else 
        {
            return time_ev->expire - now;
        }
    }
    return 1000 * 60 ;
}

static inline void __xs_ev_quit()
{
    if(g_process.quit != 0)
        return;

    g_process.quit = 1;
    if(g_process._quit)
        g_process._quit();
}

void xs_ev_do_msg(xs_ev_t* ev)
{
    xs_ev_msg_t* msg = XS_MSG_EV(ev);

    if(msg->type == XS_EV_MSG_MEM_CHECK)
    {
#ifdef XS_USE_MEMPOOL
        int i;
        xs_thread_t* thre;
        int count = 0;
        xs_mempool_check_mem_leak(g_process.mp);

        for(i=1; i<XS_MAX_THRE; i++)
        {
            thre = g_process.thre[i];
            if(thre)
            {
                xs_ev_msg_t* m = xs_new_msg_ev(XS_EV_MSG_MEM_CHECK, 0, NULL, 0);
                xs_list_add_tail(&thre->ev_recv, &m->ev->node);
                ++count;
                if(count == g_process.thre_count)
                    break;
            }
        }
#endif
        xs_free(msg->ev);
    }
}

void __xs_handle_thre_ev(xs_thread_t* th)
{
    xs_ev_t* ev;
    while(!xs_list_empty(&th->ev_send))
    {
        ev = xs_list_first_entry(&th->ev_send, xs_ev_t, node);
        xs_list_del(&ev->node);

        switch(ev->type)
        {
            case XS_EV_FREE:
                xs_free(ev);
                break;

            case XS_EV_TIME:
                xs_heap_add(g_process.timer, XS_TIME_EV(ev));
                break;

            case XS_EV_QUIT:
                xs_free(ev);
                __xs_ev_quit();
                
                break;

            case XS_EV_ADD_THRE:
                xs_free(ev);
                if(g_process.quit != 0)
                {
                    break;
                }

                xs_ev_add_thre();
                break;

            case XS_EV_KILL_THRE:
                xs_free(ev);
                if(g_process.thre_count <= 0)
                {
                    break;
                }
                
                xs_ev_kill_thre();
                break;
            case XS_EV_MSG:
                xs_ev_do_msg(ev);
                break;

            default:
                xs_loge("ev->type error %d, ev=%p", ev->type, ev);
                xs_free(ev);
                break;
        }
    }
}

void __xs_handle_thre_stop(xs_ev_t* ev)
{
    xs_ev_thre_t* thev = XS_THRE_EV(ev);
    xs_thread_t* th = g_process.thre[thev->thid];
#ifdef WIN32
    char buf;
#else
    signed char buf;
#endif
    xs_logd("now handle thre ev fd=%d, thid=%d", thev->fd, thev->thid);

    recv(thev->fd, &buf, 1, 0);

    __xs_handle_thre_ev(th);

    if(buf < 0)
    {
        /* thre already quit */
        if(!xs_list_empty(&th->ev_recv))
        {
            xs_loge("th->ev_recv is not null, maybe error");
            exit(0);
        }

        -- g_process.thre_count;

        g_process.thre[thev->thid] = NULL;
#ifdef XS_USE_MEMPOOL
        xs_mempool_destroy(th->mp);
#endif
        xs_close_socket(th->fd);
        xs_close_socket(th->thev->fd);

        xs_free(th->thev->ev);
        xs_free(th);

    }
    else
    {
        /* buf is not < 0, mean thread still running */
        if(!xs_list_empty(&th->ev_recv))
        {
            xs_list_append(&th->ev_que, &th->ev_recv);
            __xs_wakeup(th); 
        }
        else if(!xs_list_empty(&g_process.ev_recv1))
        {
            __xs_ev_trans(&th->ev_que, &g_process.ev_recv1);
            __xs_wakeup(th);
        }
        else
        {
            /* stopped  */
            xs_list_add_tail(&g_process.thre_stop, &th->node);
        }

#ifdef WIN32
        xs_ev_add(th->thev->ev);
#endif
    }
}

void __xs_handle_epoll_ev(struct epoll_event* epev, int count)
{
    int i;
    xs_ev_t* ev;
    for(i=0; i<count; ++i)
    {
        ev = epev[i].data.ptr;
        if(ev->type == XS_EV_SOCK)
        {
            xs_ev_sock_t* sev = XS_SOCK_EV(ev);
            xs_logd("now ev come fd = %d", sev->fd);
            epoll_ctl(g_process.epoll_fd, EPOLL_CTL_DEL, sev->fd, NULL); 
            /* if not quiting, add socket event to main queue
             * other wise, do not add it, app need use timer to monitor socket */
            if(!xs_ev_quiting())
                // 把任务缓存到主线程
                __xs_ev_recv(&g_process.ev_recv1, &ev->node);
        }
        else if(ev->type == XS_EV_THRE)
        {
            __xs_handle_thre_stop(ev);
        }
        else
        {
            xs_loge("ev type error=%d", ev->type);
            exit(1);
        }
    }
}

int xs_ev_init(int thre_count, void(*quit)())
{
    int i;

    g_process.mp = g_mp;
    g_process._quit = quit;

#if XS_EV_TRANS_ALL
#else
    g_process.ev_count = 0;
#endif

    /* init process */
    g_process.epoll_fd = epoll_create(1024);
    xs_list_init(&g_process.ev_recv1);
    xs_list_init(&g_process.thre_stop);
    memset(g_process.thre, 0, sizeof(xs_thread_t*)*XS_MAX_THRE);
    g_process.thre_count = 0;
    g_process.quit = 0;
    // 创建定时器堆
    g_process.timer = xs_heap_create(1024, __xs_timer_cmp);

    /* start thread */ 
    for(i = 0; i < thre_count; i++)
    {
        xs_ev_add_thre();
    }

    return 0;
}

void __xs_wakeup_stop_thread_with_ev()
{
    xs_thread_t* th;
    /* send all pending message to worker  */
#if 0
    if(!xs_list_empty(&g_process.ev_recv1)
            && !xs_list_empty(&g_process.thre_stop))
    {
        th = xs_list_first_entry(&g_process.thre_stop, xs_thread_t, node);
        xs_list_del(&th->node);
        if(!xs_list_empty(&th->ev_recv))
        {
            xs_list_append(&th->ev_que, &th->ev_recv);
        }
        __xs_ev_trans(&th->ev_que, &g_process.ev_recv1);
        __xs_wakeup(th);
    }
#endif
    xs_list_t* node = g_process.thre_stop.next;
    while(node != &g_process.thre_stop)// 便利已经停止的线程
    {
        th = xs_entry(node, xs_thread_t, node);
        node = node->next;

        /* if the thread has message, start it  */
        if(!xs_list_empty(&th->ev_recv))
        {
            // 把接收到的任务，转移到工作队列
            xs_list_append(&th->ev_que, &th->ev_recv);
            // 把线程从STOP线程队列中删除
            xs_list_del(&th->node);
            // 唤醒线程
            __xs_wakeup(th);
            continue;
        }

        /* if main thread has message, trans it to thread, and start thread */
        if(!xs_list_empty(&g_process.ev_recv1))
        {
            __xs_ev_trans(&th->ev_que, &g_process.ev_recv1);
            xs_list_del(&th->node);
            __xs_wakeup(th);
            continue;
        }
    }
}

void __xs_check_quit_flag()
{
    xs_thread_t* th;
    if(g_process.quit && xs_heap_empty(g_process.timer))
    {
        while(!xs_list_empty(&g_process.thre_stop))
        {
            th = xs_list_first_entry(&g_process.thre_stop, xs_thread_t, node);
            xs_list_del(&th->node);
            th->quit = 1;

            if(!xs_list_empty(&th->ev_recv))
                xs_list_append(&th->ev_que, &th->ev_recv);

            __xs_wakeup(th);
        }
    }
}

int xs_ev_run()
{
    int ret, wait;
    int evc = 8;
    struct epoll_event* ev = xs_malloc(sizeof(*ev) * evc);

    while(g_process.thre_count)
    {
        // 得到最近一个定时器超时时刻，距离当前时间的时间间隔，以毫秒为单位
        wait = __xs_timer_check();

        __xs_wakeup_stop_thread_with_ev();
        __xs_check_quit_flag();

        ret = epoll_wait(g_process.epoll_fd, ev, evc, wait);
        xs_logd("wait=%d, ret=%d", wait, ret);
        
        if(ret > 0)
        {
            __xs_handle_epoll_ev(ev, ret);
         //   __xs_wakeup_stop_thread_with_ev();

            /* expend ev, recv more ev in epoll */
            if(evc == ret)
            {
                evc <<= 1;
                xs_free(ev);
                ev = xs_malloc(sizeof(*ev) * evc);
            }
        }
        else if(ret < 0 && errno != EINTR)
        {
#ifdef WIN32
            xs_loge("epoll wait error=%u", WSAGetLastError());
            exit(1);

#else
            xs_loge("epoll wait error=%d", errno);
#endif
        }
    }

    xs_free(ev);
    xs_close_epoll(g_process.epoll_fd);
    xs_heap_destroy(g_process.timer);
    
    return 0;
}

int xs_ev_add_thre()
{
    xs_mempool_t* mp = pthread_getspecific(g_thread_key);
    if(mp->rtid == 0)
    {
        return __xs_ev_add_thre();
    }

    xs_thread_t* th = g_process.thre[mp->rtid];
    xs_ev_t* ev = xs_malloc(sizeof(xs_ev_t));
    XS_EV_INIT(ev, XS_EV_ADD_THRE);
    xs_list_add_tail(&th->ev_send, &ev->node);
    return 0;
}

int xs_ev_kill_thre()
{
    xs_mempool_t* mp = pthread_getspecific(g_thread_key);
    if(mp->rtid == 0)
    {
        return __xs_ev_kill_thre();
    }

    xs_thread_t* th = g_process.thre[mp->rtid];
    xs_ev_t* ev = xs_malloc(sizeof(xs_ev_t));
    XS_EV_INIT(ev, XS_EV_KILL_THRE);
    xs_list_add_tail(&th->ev_send, &ev->node);
    return 0;
}

int xs_ev_add1(xs_ev_t* ev, const char* file, int line)
{
    /* free msg, just free it */
    if(ev->type == XS_EV_FREE)
    {
        xs_free(ev);
    }
    /* sock event, just add it to epoll */
    else if(ev->type == XS_EV_SOCK)
    {
        int ret = 0;
        struct epoll_event epev;
        xs_ev_sock_t* sock = XS_SOCK_EV(ev);
        epev.data.ptr = sock->ev;  // ev
        epev.events = sock->event; // EPOLLIN


        if(epoll_ctl(g_process.epoll_fd, EPOLL_CTL_MOD, sock->fd, &epev) < 0)
            ret = epoll_ctl(g_process.epoll_fd, EPOLL_CTL_ADD, sock->fd, &epev);
        if(ret < 0)
            xs_loge("add to poll error fd=%d, file=%s, line=%d, err=%s", sock->fd, file, line, strerror(errno));
        xs_logd("add socket to ev poll fd=%d", sock->fd);
    }
    /* thread event, just add to epoll */
    else if(ev->type == XS_EV_THRE)
    {
        struct epoll_event epev;
        xs_ev_thre_t* thre = XS_THRE_EV(ev);
        epev.data.ptr = thre->ev;
        epev.events = EPOLLIN;
#ifdef WIN32
        if(epoll_ctl(g_process.epoll_fd, EPOLL_CTL_ADD, thre->fd, &epev) < 0)
            epoll_ctl(g_process.epoll_fd, EPOLL_CTL_MOD, thre->fd, &epev);
#else
        xs_logd("thre add to ev poll");
        epoll_ctl(g_process.epoll_fd, EPOLL_CTL_ADD, thre->fd, &epev);
#endif
    }
    /* other msg */
    else
    {
        xs_mempool_t* mp = pthread_getspecific(g_thread_key);
        if(mp->rtid == 0)
        {
            if(ev->type == XS_EV_TIME)
            {
                xs_ev_time_t* tev = XS_TIME_EV(ev);
                xs_heap_add(g_process.timer, tev);
            }
            else if(ev->type == XS_EV_ADD_THRE)
            {
                xs_free(ev);
                __xs_ev_add_thre();
            }
            else if(ev->type == XS_EV_KILL_THRE)
            {
                xs_free(ev);
                __xs_ev_kill_thre();
            }
            else if(ev->type == XS_EV_QUIT)
            {
                xs_free(ev);
                __xs_ev_quit();
            }
            else if (ev->type == XS_EV_MSG)
            {
                xs_ev_do_msg(ev);
            }
        }
        else
        {
            xs_thread_t* th = (xs_thread_t*)mp->rtdata;
            xs_list_add_tail(&th->ev_send, &ev->node);
        }
    }
    return 0;
}

int xs_ev_quit()
{
    xs_ev_t* ev = xs_malloc(sizeof(xs_ev_t));
    XS_EV_INIT(ev, XS_EV_QUIT);
    xs_ev_add(ev);
    return 0;
}


xs_ev_time_t* xs_ev_add_time_ev(int period, void(*func)(xs_ev_time_t*), void* ptr)
{
    xs_ev_time_t* t = xs_new_time_ev(period, func, ptr);
    xs_ev_add(t->ev);
    return t; 
}

xs_ev_sock_t* xs_ev_add_sock_ev1(XSOCKET fd, int event, void(*func)(xs_ev_sock_t*), void* ptr, const char* file, int line)
{
    xs_ev_sock_t* s = xs_new_sock_ev1(fd, event, func, ptr, file, line);
    // 把socket加入到epoll
    xs_ev_add1(s->ev, file, line);
    return s;
}

xs_ev_msg_t* xs_ev_add_msg_ev1(int type, int thid, void(*func)(xs_ev_msg_t*), void* ptr, const char* file, int line)
{
    xs_ev_msg_t* msg = xs_new_msg_ev1(type, thid, func, ptr, file, line);
    xs_ev_add1(msg->ev, __FILE__, __LINE__);
    return msg;    
}

void xs_ev_time_restart(xs_ev_time_t* ev, int period)
{
    ev->expire = __xs_timer_now() + period;
    ev->period = period;
    ev->cancel = 0;
    xs_ev_add(ev->ev);
}
#ifdef __cplusplus
}
#endif
