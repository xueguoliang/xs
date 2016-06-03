
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

#ifdef WIN32
char *strtok_r(char *s, const char *delim, char **save_ptr) {     
    char *token;     

    if (s == NULL) s = *save_ptr;     

    /* Scan leading delimiters.  */     
    s += strspn(s, delim);     
    if (*s == '\0')
        return NULL;     

    /* Find the end of the token.  */     
    token = s;     
    s = strpbrk(token, delim);     
    if (s == NULL)     
        /* This token finishes the string.  */     
        *save_ptr = strchr(token, '\0');
    else {     
        /* Terminate the token and make *SAVE_PTR point past it.  */     
        *s = '\0';
        *save_ptr = s + 1;     
    }     

    return token;     
} 

int socketpair(int af, int type, int protocol, XSOCKET socks[])
{
    protocol = protocol;
    XSOCKET listener, connector, acceptor;
    struct sockaddr_in listen_addr, connect_addr;
    int size = sizeof(connect_addr);

    listener = socket(af, type, 0);
    if(listener == (XSOCKET)SOCKET_ERROR)
        return -1;

    memset(&listen_addr, 0, sizeof(listen_addr));
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    listen_addr.sin_port = 0;

    if(bind(listener, (struct sockaddr*)&listen_addr, sizeof(listen_addr)) == -1)
    {
        xs_close_socket(listener);
        return -1;
    }

    if(listen(listener, 1) == -1)
    {
        xs_close_socket(listener);
        return -1;
    }


    if(getsockname(listener, (struct sockaddr*)&connect_addr, &size) == -1)
    {
        xs_close_socket(listener);
        return -1;
    }
    connector = socket(af, type, 0);
    if(connector == (XSOCKET)SOCKET_ERROR)
    {
        xs_close_socket(listener);
        return -1;
    }

    if(connect(connector, (struct sockaddr*)&connect_addr, size) == -1)
    {
        xs_close_socket(connector);
        xs_close_socket(listener);
        return -1;
    }

    size = sizeof(listen_addr);
    acceptor = accept(listener, (struct sockaddr*)&listen_addr, &size);
    if(acceptor == (XSOCKET)SOCKET_ERROR)
    {
        xs_close_socket(connector);
        xs_close_socket(listener);
        return -1;
    }

    socks[0] = connector;
    socks[1] = acceptor;

    xs_logd("socketpair=%d, %d", socks[0], socks[1]);

    return 0;
}

int inet_pton(int af, const char *src, void *dst)
{
    if(af == AF_INET6)
        assert(0);

    struct	in_addr* addr = (struct in_addr*)dst;
    addr->S_un.S_addr = inet_addr(src);

    return 0;
}

/* use select do epoll */
typedef struct xs_epoll_event_t
{
    XSOCKET fd;
    struct epoll_event ev;
    xs_list_t lnode;
    struct rb_node rb;
} xs_epoll_event_t;

typedef struct xs_epoll_evs_t
{
    xs_list_t ready;
    xs_list_t go;
    struct rb_root tree;
} xs_epoll_evs_t;

pthread_mutex_t g_epoll_mutex = PTHREAD_MUTEX_INITIALIZER;
xs_epoll_evs_t* g_epoll_evs = NULL;
#define xs_epoll_lock() pthread_mutex_lock(&g_epoll_mutex)
#define xs_epoll_unlock() pthread_mutex_unlock(&g_epoll_mutex)
#define xs_epoll_find(_tree, _key) rb_find(_tree, (const void*)(intptr_t)_key)

int xs_epoll_evs_cmp(const struct rb_node* node, const void* ptr)
{
    xs_epoll_event_t* ev = rb_entry(node, xs_epoll_event_t, rb);
 //   xs_logd("%d, key=%d", ev->fd, (XSOCKET)(intptr_t)ptr);
    return ev->fd - (XSOCKET)(intptr_t)ptr;
}

xs_epoll_event_t* xs_epoll_event_create(XSOCKET fd, struct epoll_event* ev)
{
    xs_epoll_event_t* xsev = xs_malloc(sizeof(xs_epoll_event_t));
    memcpy(&xsev->ev, ev, sizeof(*ev));
    xsev->fd = fd;
    rb_init_node(&xsev->rb);
    xs_list_init(&xsev->lnode);
    return xsev;
}

int epoll_ctl(int epollfd, int method, XSOCKET fd, struct epoll_event* ptr)
{
    int ret = 0;
    struct rb_node* node;
    xs_epoll_event_t* xsev;

    epollfd = epollfd;

    xs_epoll_lock();
    if(g_epoll_evs == NULL)
    {
        g_epoll_evs = xs_malloc(sizeof(xs_epoll_evs_t));
        xs_list_init(&g_epoll_evs->go);
        xs_list_init(&g_epoll_evs->ready);
        rb_init(&g_epoll_evs->tree, xs_epoll_evs_cmp);
    }
    node = xs_epoll_find(&g_epoll_evs->tree, fd);

    if(node)
    {
        xsev = xs_entry(node, xs_epoll_event_t, rb);
        switch (method)
        {
            case EPOLL_CTL_DEL:
                rb_erase(&xsev->rb, &g_epoll_evs->tree);
                xs_list_del(&xsev->lnode);
                goto RET;

            case EPOLL_CTL_MOD:
                memcpy(&xsev->ev, ptr, sizeof(*ptr));
                xs_list_move_tail(&g_epoll_evs->ready, &xsev->lnode);
                goto RET;

            case EPOLL_CTL_ADD:
                /* already exist, can not add again */
                xs_logd("already exist fd=%d", xsev->fd);
                ret = -1;
                goto RET;
        }
    }
    else
    {
        switch (method)
        {
            /* not exist, can not del and mod */
            case EPOLL_CTL_DEL:
            case EPOLL_CTL_MOD:
                ret = -1;
                goto RET;
            case EPOLL_CTL_ADD:
                /* add the event in ready list */
                xsev = xs_epoll_event_create(fd, ptr);
                rb_insert(&xsev->rb, &g_epoll_evs->tree, (void*)(intptr_t)fd);
                xs_list_add_tail(&g_epoll_evs->ready, &xsev->lnode);
                goto RET;
        }
    }
RET:
    xs_epoll_unlock();
    return ret;
}

int epoll_wait(int epollfd, struct epoll_event* ev_ret, int count, int t)
{
    int i;
    xs_list_t* node;
    xs_epoll_event_t* ev;
    struct timeval tv;
    fd_set r;
    fd_set w;
    FD_ZERO(&r);
    FD_ZERO(&w);

    xs_epoll_lock();
    if(!xs_list_empty(&g_epoll_evs->go))
        goto RET;

    epollfd = epollfd;
    node = g_epoll_evs->ready.next;
    while(node != &g_epoll_evs->ready)
    {
        ev = xs_entry(node, xs_epoll_event_t, lnode);
        node = node->next;
        if(ev->ev.events & EPOLLIN)
        {
            FD_SET(ev->fd, &r);
        }
        if(ev->ev.events & EPOLLOUT)
        {
            FD_SET(ev->fd, &w);
        }
    } 
    xs_epoll_unlock();


    tv.tv_sec = t/1000;
    tv.tv_usec = t%1000*1000;

    int ret = select(1024, &r, &w, NULL, &tv);
    if(ret <= 0)
        return ret;

    xs_epoll_lock();

    if(!xs_list_empty(&g_epoll_evs->ready))
    {
        node = g_epoll_evs->ready.next;
        while(node != &g_epoll_evs->ready)
        {
            ev = xs_list_entry(node, xs_epoll_event_t, lnode);
            node = node->next;
            if(FD_ISSET(ev->fd, &r) || FD_ISSET(ev->fd, &w))
            {
                xs_list_move_tail(&g_epoll_evs->go, &ev->lnode);
            }
        }
    }

RET:
    for(i=0; i<count; ++i)
    {
        if(xs_list_empty(&g_epoll_evs->go))
            break;
        
        ev = xs_list_first_entry(&g_epoll_evs->go, xs_epoll_event_t, lnode);
        xs_list_del(&ev->lnode);
        memcpy(&ev_ret[i], &ev->ev, sizeof(struct epoll_event));
    }
    xs_epoll_unlock();
    return i;
}

int epoll_create(int count)
{
    g_epoll_evs = xs_malloc(sizeof(xs_epoll_evs_t));
    xs_list_init(&g_epoll_evs->go);
    xs_list_init(&g_epoll_evs->ready);
    rb_init(&g_epoll_evs->tree, xs_epoll_evs_cmp);
    count = count;
    return 0;
}

int xs_close_epoll(int fd)
{
    fd = fd;
    return 0;
}

#endif

void xs_epoll_add(int epoll, XSOCKET sock, uint32_t events, void* ptr)
{
    struct epoll_event ev;
    if(ptr)
        ev.data.ptr = ptr;
    else
        ev.data.fd = sock;
    ev.events = events;
    epoll_ctl(epoll, EPOLL_CTL_ADD, sock, &ev);
}
#ifdef __cplusplus
}
#endif
