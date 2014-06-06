
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

void* xs_model_alloc(int argc)
{
    xs_model_t* m = xs_zalloc(sizeof(xs_model_t) + argc * sizeof(char*));
    m->argc = argc;
    return m;
}

xs_model_t* xs_model_create(int argc, ...)
{
    va_list ap;
    int i;
    va_start(ap, argc);
    xs_model_t* m = xs_zalloc(sizeof(xs_model_t) + argc * sizeof(char*));
    m->argc = argc;
    for(i=0; i<argc; i++)
    {
        m->argv[i] = xs_strdup(va_arg(ap, char*));
    }

    va_end(ap);
    return m;
}

int xs_model_call(int fd, xs_model_t** r, int argc, ...)
{
    int ret;
    va_list ap;
    int i;
    va_start(ap, argc);
    xs_model_t* m = xs_zalloc(sizeof(xs_model_t) + argc * sizeof(char*));
    m->argc = argc;
    for(i=0; i<argc; i++)
    {
        m->argv[i] = xs_strdup(va_arg(ap, char*));
    }
     va_end(ap);

     ret = xs_model_send_block(m, fd, 60000);
     if(ret != 0)
         return ret;

     xs_logi("send success");

     ret = xs_model_recv_block(&m, fd, 60000);
     if(ret < 0)
         return ret;

     xs_logi("recv success");
     *r = m;

     return ret;
}

void xs_model_rsp_callback(int fd, void(*callback)(xs_aio_t*), xs_ctrl_t* ctrl, int argc, ...)
{
    va_list ap;
    xs_model_t* m;
    va_start(ap, argc);
    m = xs_model_createv(argc, ap);
    va_end(ap);

    char* buf; int size;
    buf = xs_model_to_buf_and_free_model(m, &size);
    xs_aio_send_with_header(fd, callback, buf, size, ctrl);
}

void xs_model_delete(xs_model_t* model)
{
    int i;
    for(i=0; i<model->argc; i++)
        xs_free(model->argv[i]);
    xs_free(model);
}

xs_model_t* xs_model_from_buf(char* buf, int size)
{
    xs_dict_t* dict;
    xs_model_t* model;
    char* argc;
    int i;
    size = size;
    
    dict = xs_dict_from_buf(buf, NULL); 
    argc = xs_dict_find_value(dict, __xs_argc); /* now argc == 2 */
    model = xs_model_alloc(atoi(argc));

    for(i=0; i<model->argc; i++)
    {
        char buf[64];
        model->argv[i] = xs_dict_str(dict, xs_itoa_r(i, buf)); /* argv[0] = clis, argv[1] = "init" */
    }
    xs_dict_destroy(dict);
    return model;
}

int xs_model_recv_block(xs_model_t** model, int fd, int timeout)
{
    int size;
    char* buf;
    int ret;
    
    *model = NULL;

    ret = xs_sock_recv_block(fd, &size, 4, timeout);
    if(ret != 4)
    {
        return -1;
    }

    size = ntohl(size);
    buf = (char*)xs_malloc(size+1);
    buf[size] = 0;

    ret = xs_sock_recv_block(fd, buf, size, timeout);
    if(ret == size)
    {
        *model = xs_model_from_buf(buf, size);
        xs_free(buf);
        return 0;
    }

    xs_free(buf);
    return -1;
}

int xs_model_send_block(xs_model_t* model, int fd, int timeout)
{
    char* buf;
    int size;
    int ret;
    buf = xs_model_to_buf((xs_model_t*)model, &size);
    xs_logd("buf=%s", buf+4);
    ret = xs_sock_send_block(fd, buf, size, timeout);
    xs_free(buf);
    if(ret != size)
        return -1;
    return 0;
}

char* xs_model_to_buf(xs_model_t* model, int* size)
{
    char buf[32];
    xs_dict_t* dict = NULL;
    int i;
    
    dict = xs_dict_add_int(dict, __xs_argc, model->argc);
    for(i=0; i<model->argc; i++)
    {
        if(model->argv[i])
            dict = xs_dict_add_str(dict, xs_itoa_r(i, buf), model->argv[i]);
    }

    return xs_dict_to_buf_with_head(dict, size);
}

/****************model recv and send 5.29****************/
void xs_model_aio_recv_done(xs_aio_t* aio)
{
    xs_model_cb* mcb = aio->ptr;

    if(aio->result == XS_AIO_DONE)
    {
        xs_model_t* model = xs_model_from_buf(aio->buf, aio->buflen);
        mcb->callback_func(model, mcb->ptr, aio->result);
        xs_model_delete(model);
    }
    else
    {
        mcb->callback_func(NULL, mcb->ptr, aio->result);
    }
    xs_free(mcb);
    xs_aio_free(aio);
}
void xs_model_recv(int fd, model_callback_func_t func, void* ptr)
{
    xs_model_cb* mcb = xs_malloc(sizeof(*mcb));
    mcb->callback_func = func;
    mcb->ptr = ptr;
    mcb->model = NULL;
    xs_aio_recv(fd, xs_model_aio_recv_done, mcb); 
}

void xs_model_aio_send_done(xs_aio_t* aio)
{
    xs_model_cb* mcb = aio->ptr;
    mcb->callback_func(mcb->model, mcb->ptr, aio->result);
    xs_model_delete(mcb->model);
    xs_free(mcb);
    xs_aio_free(aio);
}

void xs_model_send(int fd, xs_model_t* model, model_callback_func_t func, void* ptr)
{
    xs_logd("enter model send");
    xs_model_cb* mcb = xs_malloc(sizeof(*mcb));
    mcb->callback_func = func;
    mcb->ptr = ptr;
    mcb->model = model;
    int len;
    char* buf = xs_model_to_buf(model, &len);
    xs_logd("buf is:%s", buf+4);
    xs_aio_send_with_header(fd, xs_model_aio_send_done, buf, len, mcb);
}


#ifdef __cplusplus
}
#endif
