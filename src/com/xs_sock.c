
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

void xs_sock_close(xs_ev_sock_t* sev)
{
    xs_logi("socket fd=%d closed", sev->fd);
    xs_close_socket(sev->fd);
    xs_free(sev->ev);
}

#ifdef WIN32
// in windows, i have no way to resolve getaddrinfo and freeaddrinfo
#else
struct in_addr xs_sock_getip(const char* name)
{
    struct addrinfo hints;
    struct addrinfo* res; /*, *cur;*/
    struct sockaddr_in* addr;
    struct in_addr ret;
    int result;
    ret.s_addr = INADDR_NONE;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family     = AF_INET;
    hints.ai_flags      = AI_PASSIVE;
    hints.ai_protocol   = 0;
    hints.ai_socktype   = SOCK_STREAM;

    result = getaddrinfo(name, NULL, &hints, &res);
    if(result < 0)
    {
        return ret;
    }

#if 0
    for(cur = res; cur != NULL; cur = cur->ai_next)
    {
        addr = (struct sockaddr_in*)cur->ai_addr;
        ret = addr.sin_addr.s_addr;
    }
#endif
    addr = (struct sockaddr_in*)res->ai_addr;
    ret = addr->sin_addr;
    freeaddrinfo(res);
    return ret;
}
#endif


int xs_sock_connect(uint16_t port, const char* ip)
{
    int fd;
    int ret;
    struct sockaddr_in addr;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0)
        return fd;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
   // inet_pton(AF_INET, ip, &addr.sin_addr); 
#ifdef WIN32
    addr.sin_addr.s_addr = inet_addr(ip);
#else
    addr.sin_addr = xs_sock_getip(ip);
#endif

    ret = connect(fd, (struct sockaddr*)&addr, sizeof(addr));
    if(ret < 0 && errno != EINPROGRESS)
    {
        xs_close_socket(fd);
        return ret;
    }
    return fd;
}

int xs_sock_send_block(XSOCKET fd, const void* buf, int size, int timeout)
{
    int ret;
    int io = 0;
    fd_set set;
    struct timeval tv;
    
    if(timeout == 0) timeout = 60000;
    tv.tv_sec = timeout/1000;
    tv.tv_usec = timeout%1000*1000;
    
    while(io < size)
    {
        FD_ZERO(&set);
        FD_SET(fd, &set);

        if(timeout >= 0)
            ret = select(fd+1, NULL, &set, NULL, &tv);
        else
            ret = select(fd+1, NULL, &set, NULL, NULL);

        if(ret > 0)
        {
#ifdef WIN32
            ret = send(fd, buf+io, size-io, 0);
#else
            ret = write(fd, buf+io, size-io);
#endif
            if(ret > 0)
            {
                io += ret;
                continue;
            }
            if(ret < 0 && errno == EAGAIN)
                continue;
        }

        break;
    }
    return io;
}

int xs_sock_recv_block(XSOCKET fd, void* buf, int size, int timeout)
{
    int ret;
    int io = 0;
    fd_set set;
    struct timeval tv;
    if(timeout == 0) timeout = 60000;
    tv.tv_sec = timeout/1000;
    tv.tv_usec = timeout%1000*1000;
    
    while(io < size)
    {
        FD_ZERO(&set);
        FD_SET(fd, &set);

        if(timeout >= 0)
            ret = select(fd+1, &set, NULL, NULL, &tv);
        else
            ret = select(fd+1, &set, NULL, NULL, NULL);

        if(ret > 0)
        {
#ifdef WIN32
            ret = recv(fd, buf+io, size-io, 0);
#else
            ret = read(fd, buf+io, size-io);
#endif
            if(ret > 0)
            {
                io += ret;
                continue;
            }
            if(ret < 0 && errno == EAGAIN)
                continue;
        }

        break;
    }
    return io;
}
#ifdef __cplusplus
}
#endif
