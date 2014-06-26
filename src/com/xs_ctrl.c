
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
void xs_ctrl_accept(xs_ev_sock_t* ev)
{
    int fd;
    xs_ctrl_t* ctrl = ev->arg;

    while((fd = accept(ev->fd, NULL, NULL)) > 0)
    {
        xs_logi("accept");
        ctrl->r(fd, ctrl);
    }

    if(errno == EAGAIN)
    {
        xs_ev_add(ev->ev);
    }
    else
    {
        xs_loge("server error");
        xs_ctrl_destroy(ev->arg);
    }
}

xs_ctrl_t* xs_ctrl_create(uint16_t port, 
        xs_ctrl_recv_t r, xs_ctrl_handle_t h)
{
    xs_ctrl_t* ctrl = xs_malloc(sizeof(*ctrl));

    ctrl->objs = xs_hash_create();
    ctrl->port = port;
    ctrl->server = NULL;
    if(r == NULL)
        r = xs_ctrl_recv_df;
    if(h == NULL)
        h = xs_ctrl_handle_df;
    ctrl->r = r;
    ctrl->h = h;
    
    if(ctrl->port > 0)
        ctrl->server = xs_server_start_tcp(port, 10, xs_ctrl_accept, ctrl);

    return ctrl;
}

void xs_ctrl_recv_df_cbk(xs_aio_t* aio)
{
    xs_ctrl_t* ctrl = aio->ptr;
    if(aio->result == XS_AIO_DONE)
    {
        xs_logi("buf=%s", aio->buf);
        ctrl->h(aio->fd, aio->buf, aio->buflen, ctrl);
    }
    else
    {
        xs_logd("a ,close socket ******************************* no");
        xs_close_socket(aio->fd);
    }
    xs_aio_free(aio);
}

void xs_ctrl_recv_df(int fd, xs_ctrl_t* ctrl)
{
    xs_logi("recv now");
    xs_aio_recv(fd, xs_ctrl_recv_df_cbk, ctrl);
}

void xs_ctrl_handle_df(int fd, char* buf, int size, xs_ctrl_t* ctrl)
{
    size = size;
    xs_model_t* model;
    xs_logd("recv buf=%s", buf);
    model = xs_model_from_buf(buf); /* 0`clis`1`init`argc`2 */ 
    xs_object_t* obj = xs_hash_find(ctrl->objs, model->argv[0]);
    
    if(obj)
    {
        xs_object_handler_t handle = xs_hash_find(obj, model->argv[1]);
        if(handle)
        {
            handle(model, fd, ctrl);
        }
        else
        {
            // bad guy
            xs_loge("can not find operation %s", model->argv[1]);
            xs_close_socket(fd);
        }
    }
    else
    {
        xs_loge("can not find object %s", model->argv[0]);
        xs_close_socket(fd);
    }
    xs_model_delete(model);
}

void xs_ctrl_object_clear(xs_hash_node_t* node)
{
    xs_hash_destroy(node->value, NULL);
}

void xs_ctrl_destroy(xs_ctrl_t* ctrl)
{
    if(ctrl->objs)
        xs_hash_destroy(ctrl->objs, xs_ctrl_object_clear);
    if(ctrl->server)
        xs_sock_close(ctrl->server);

    xs_free(ctrl);
}

#if 0
int xs_ctrl_send_block_ip(xs_ctrl_t* ctrl, char* ip, char* buf, int size)
{
    int fd;
    int ret;

    fd = xs_sock_connect(ctrl->port, ip);
    if(fd < 0)
    {
        return fd; 
    }

    ret = xs_ctrl_send_block(ctrl, fd, buf, size);
    if(ret != fd)
        xs_close_socket(fd);
    return ret;
}
int xs_ctrl_send_block(xs_ctrl_t* ctrl, int fd, char* buf, int size)
{
    int ret;
    ret = xs_sock_send_block(fd, buf, size, ctrl->timeout);
    if(ret == size)
        return fd;
    return -1;
}

char* xs_ctrl_recv_block(xs_ctrl_t* ctrl, int fd, int* size)
{
    int head;
    int ret;
    char* buf;
    ret = xs_sock_recv_block(fd, (char*)&head, sizeof(head), ctrl->timeout);    
    if(ret != sizeof(head))
        return NULL;
    head = ntohl(head);

    buf = (char*)xs_malloc(head+1);
    buf[head] = 0;
    ret = xs_sock_recv_block(fd, buf, head, ctrl->timeout);
    if(ret != head)
    {
        xs_free(buf);
        return NULL;
    }

    *size = head;

    return buf;
}

char* xs_ctrl_recv_block_ip(xs_ctrl_t* ctrl, char* ip, int* size)
{
    int fd = xs_sock_connect(ctrl->port, ip);
    if(fd < 0)
    {
        return NULL;
    }
    char* buf = xs_ctrl_recv_block(ctrl, fd, size);
    if(!buf)
    {
        xs_close_socket(fd);
    }
    return buf;
}
#endif

#ifdef __cplusplus
}
#endif
