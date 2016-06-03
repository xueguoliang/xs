
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
#ifndef __XS_NET_H__
#define __XS_NET_H__


#ifdef WIN32
/* select model in win32 */

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET XSOCKET;
#define EINPROGRESS WSAEWOULDBLOCK
char *strtok_r(char *s, const char *delim, char **save_ptr);
#define EPOLLIN 1
#define EPOLLOUT 2
#define EPOLL_CTL_DEL 0
#define EPOLL_CTL_MOD 1
#define EPOLL_CTL_ADD 2
struct epoll_event
{
    union
    {
        void* ptr;
    } data;
    int events;
};

int epoll_ctl(int p, int m, XSOCKET fd, struct epoll_event* ptr);
int epoll_create(int c);
int epoll_wait(int p, struct epoll_event* ev, int c, int tw);
int xs_close_epoll(int epollfd);
#define xs_close_socket closesocket
int inet_pton(int af, const char *src, void *dst);

int socketpair(int af, int type, int protocol, XSOCKET socks[2]);

#else

#include <sys/epoll.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>
#define xs_close_epoll close

#define xs_close_socket(_fd) do{close(_fd);}while(0)
typedef int XSOCKET;
#endif

void xs_epoll_add(int epoll, XSOCKET sock, uint32_t events, void* ptr);

#endif
#ifdef __cplusplus
}
#endif
