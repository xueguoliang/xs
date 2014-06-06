
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
#ifndef __XS_AIO_H__
#define __XS_AIO_H__

typedef struct xs_aio_t
{
    int             fd;
    char*           buf;
    int             buflen;
    int             iolen;
    void            (*func)(struct xs_aio_t*);
#define XS_AIO_DONE 1
#define XS_AIO_FAIL 0
    int             result;
    void*           ptr;
    xs_ev_sock_t*   sev;
    int             ret;
} xs_aio_t;

void xs_aio_recv(int fd, void(*func)(xs_aio_t*), void* ptr);
void xs_aio_send_with_header(int fd, void(*func)(xs_aio_t*), char* buf, int len, void* ptr);
void xs_aio_send_no_header(int fd, void(*func)(xs_aio_t*), char* buf, int len, void* ptr);
void _xs_aio_wait_close(xs_aio_t* aio);
static inline void xs_aio_send_and_close(int fd, char* buf, int len)
{
    xs_aio_send_with_header(fd, _xs_aio_wait_close, buf, len, NULL);
}

#define xs_aio_free(__aio) do{ if(!__aio)break;\
    if(__aio->buf)xs_free(__aio->buf);\
    if(__aio->sev)xs_free(__aio->sev->ev);\
    xs_free(__aio);\
}while(0)

#endif
#ifdef __cplusplus
}
#endif
