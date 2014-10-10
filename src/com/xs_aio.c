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

void __xs_aio_recv_data(xs_ev_sock_t* sev)
{
    xs_aio_t* aio = sev->arg;
    int ret;

    xs_logd("recv data sev->fd=%d, aio->iolen=%d, aio->buflen=%d", sev->fd, aio->iolen, aio->buflen);

    /* recv head */
    if(aio->iolen < 4)
    {
        char* p = (char*)&aio->buflen;
        ret = read(sev->fd, p+aio->iolen, 4-aio->iolen);

        if(ret > 0)
        {
            aio->iolen += ret;
            if(aio->iolen < 4)
            {
                xs_ev_add(sev->ev);
                return;
            }
            else if(aio->iolen == 4)
            {
                aio->buflen = ntohl(aio->buflen);
                xs_logd("recv head buffer length = %d", aio->buflen);
                aio->buf = xs_malloc(aio->buflen + 1);
                aio->buf[aio->buflen] = 0;
            }
        }
        else if (ret < 0 && errno == EAGAIN)
        {
            xs_logd("add to ev poll");
            xs_ev_add(sev->ev);
            return ;
        }
        else // ret == 0 or (ret< 0 &&  errno!=EAGAIN)
        {
            xs_logd("here ret=%d", ret);
            aio->ret = ret;
            aio->func(aio);
            return;
        }
    }

    int alreadyRecv = aio->iolen - 4;

    ret = read(sev->fd, aio->buf + alreadyRecv,
            aio->buflen - alreadyRecv);
    if(ret > 0)
    {
        aio->iolen += ret;
        if(aio->iolen == aio->buflen + 4)
        {
            aio->result = XS_AIO_DONE;
            aio->ret = ret;
            xs_logd("************************callback ok fd=%d, aio->buf=%s, aio->buflen=%d", aio->fd, aio->buf, aio->buflen);
            aio->func(aio);
        }
        else
        {
            xs_logd("here");
            xs_ev_add(sev->ev);
        }
    }
    else if(ret < 0 && errno == EAGAIN)
    {
        xs_logd("here");
        xs_ev_add(sev->ev);
    }
    else
    {
        xs_logd("here ret=%d", ret);
        aio->ret = ret;
        aio->func(aio);
    }
}

void xs_aio_recv(int fd, void(*func)(xs_aio_t*), void* ptr)
{
    xs_aio_t* aio = xs_zalloc(sizeof(*aio));
    aio->fd = fd;
    aio->buf = NULL;
    aio->buflen = 0;
    aio->iolen = 0;
    aio->func = func;
    aio->result = XS_AIO_FAIL;
    aio->ptr = ptr;

    aio->sev = xs_ev_add_sock_ev(fd, EPOLLIN, __xs_aio_recv_data, aio);
}


void __xs_aio_send_data(xs_ev_sock_t* sev)
{
    xs_aio_t* aio = sev->arg;

    int ret;

    ret = write(aio->fd, 
            aio->buf + aio->iolen,
            aio->buflen - aio->iolen);
    if (ret > 0)
    {
        aio->iolen += ret;
        if(aio->iolen == aio->buflen)
        {
            aio->result = XS_AIO_DONE;
            aio->ret = ret;
            aio->func(aio);
        }
        else
        {
            xs_ev_add(sev->ev);
        }
    }
    else if(ret < 0 && errno == EAGAIN)
    {
        xs_ev_add(sev->ev);
    }
    else /* <0 and ==0 */
    {
        xs_loge("socket send data error, ret=%d, errno=%d, fd=%d", ret, errno, aio->fd);
        aio->ret = ret;
        aio->func(aio);
    }
}

void xs_aio_send_with_header(int fd, void(*func)(xs_aio_t*), char* buf, int len, void* ptr)
{
    xs_aio_t* aio = xs_zalloc(sizeof(*aio));

    aio->fd = fd;
    aio->buf = buf;
    aio->buflen = len;
    aio->iolen = 0;
    aio->func = func;
    aio->result = XS_AIO_FAIL;
    aio->ptr = ptr;

    aio->sev = xs_ev_add_sock_ev(fd, EPOLLOUT, __xs_aio_send_data, aio);
}

void __xs_aio_send_data_no_header(xs_ev_sock_t* sev)
{
    xs_aio_t* aio = sev->arg;

    int ret;
    if(aio->iolen < 4)
    {
        int header = aio->buflen;
        header = htonl(header);
        ret = write(aio->fd, ((char*)&header) + aio->iolen, 4 - aio->iolen);
        xs_logd("ret is %d", ret);
        if(ret > 0)
        {
            aio->iolen += ret;
            if(aio->iolen < 4)
            {
                xs_ev_add(sev->ev);
                return;
            }
        }
        else if(ret < 0 && errno == EAGAIN)
        {
            xs_ev_add(sev->ev);
            return;
        }
        else
        {
            aio->ret = ret;
            aio->func(aio);
            return;
        }
    }

    ret = write(aio->fd, 
            aio->buf + (aio->iolen - 4),
            aio->buflen - (aio->iolen - 4));

    xs_logd("send data ret=%d", ret);
    if (ret > 0)
    {
        aio->iolen += ret;
        if(aio->iolen == aio->buflen + 4)
        {
            aio->result = XS_AIO_DONE;
            aio->ret = ret;
            aio->func(aio);
        }
        else
        {
            xs_ev_add(sev->ev);
        }
    }
    else if(ret < 0 && errno == EAGAIN)
    {
        xs_ev_add(sev->ev);
    }
    else /* <0 and ==0 */
    {
        xs_loge("socket send data error, ret=%d, errno=%d, fd=%d", ret, errno, aio->fd);
        aio->ret = ret;
        aio->func(aio);
    }
}

void xs_aio_send_no_header(int fd, void(*func)(xs_aio_t*), char* buf, int len, void* ptr)
{
    xs_aio_t* aio = xs_zalloc(sizeof(*aio));

    aio->fd = fd;
    aio->buf = buf;
    aio->buflen = len;
    aio->iolen = 0;
    aio->func = func;
    aio->result = XS_AIO_FAIL;
    aio->ptr = ptr;

    aio->sev = xs_ev_add_sock_ev(fd, EPOLLOUT, __xs_aio_send_data_no_header, aio);
}

void _xs_aio_wait_close(xs_aio_t* aio)
{
    if(aio->result == XS_AIO_DONE)
    {
        xs_sock_wait_peer_close(aio->fd);
    }
    else
    {
        xs_close_socket(aio->fd);
    }
    xs_aio_free(aio);
}

#ifdef __cplusplus
}
#endif
