
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

xs_model_t* xs_model_from_buf(char* buf)
{
    char* saveptr;
    char* p = strtok_r(buf, "|", &saveptr);
    xs_vec_t* vec = xs_vec_create(8);
    while(p)
    {
        xs_vec_add(vec, p);
        p = strtok_r(NULL, "|", &saveptr);
    }

    xs_logd("***************8vec->count=%d", vec->count);
    xs_model_t* model = xs_model_create(vec->count);
    model->argc = vec->count;
    int i;
    for(i=0; i<model->argc; ++i)
    {
        model->argv[i] = xs_strdup(xs_vec_get(vec, i));
    }
    xs_vec_destroy(vec, NULL);
    return model;
}

void xs_model_aio_recv_done(xs_aio_t* aio)
{
    xs_model_cb_t* cb = aio->ptr;
    cb->aio = aio;

    if(aio->result == XS_AIO_DONE)
    {
        cb->model = xs_model_from_buf(aio->buf);
        cb->func(cb);
    }
    else
    {
        cb->model = NULL;
        cb->func(cb);
    }
    xs_free(cb);
    xs_aio_free(aio);
}

void xs_model_recv(int fd, xs_model_cbk_t cbk, void* ptr)
{
    xs_model_cb_t* cb = xs_malloc(sizeof(*cb));
    cb->func = cbk;
    cb->model = NULL;
    cb->ptr = ptr;
    cb->aio = NULL;
    xs_aio_recv(fd, xs_model_aio_recv_done, cb); 
}

char* xs_model_to_buf(xs_model_t* model, int* olen)
{
    int len = 1;
    int i;
    for(i=0; i<model->argc; ++i)
    {
        len += strlen(model->argv[i]); 
        ++len;
    }
    xs_logd("****************len is %d", len);
    char* buf = xs_malloc(len+1);
    buf[len] = 0;

    len = 0;
    for(i=0; i<model->argc; ++i)
    {
        len += sprintf(buf+len, "%s|", model->argv[i]);
    }

    *olen = len+1;
    return buf;
}

void xs_model_aio_send_done(xs_aio_t* aio)
{
    xs_model_cb_t* cb = aio->ptr;
    cb->aio = aio;
    cb->func(cb);
    xs_free(cb);
    xs_aio_free(aio);
}

void xs_model_send(int fd, xs_model_cbk_t cbk, void* ptr, xs_model_t* model)
{
    int len;
    char* buf = xs_model_to_buf(model, &len);
    xs_model_cb_t* cb = xs_malloc(sizeof(*cb));
    cb->func = cbk;
    cb->model = model;
    cb->ptr = ptr;
    cb->aio = NULL;
    xs_aio_send_no_header(fd, xs_model_aio_send_done, buf, len, cb);
}
void xs_model_send_and_close(int fd, xs_model_t *model)
{
    int len;
    char* buf = xs_model_to_buf(model, &len);
//    xs_model_cb_t* cb = xs_malloc(sizeof(*cb));

    xs_aio_send_no_header_and_close(fd, buf, len);
    xs_model_delete(model);
}
xs_model_t* xs_model_clone(xs_model_t* model)
{
    xs_model_t* ret = xs_model_create(model->argc);
    int i=0;
    for(i=0;i<ret->argc; ++i)
    {
        ret->argv[i] = xs_strdup(model->argv[i]);
    }
    return ret;
}
#ifdef __cplusplus
}
#endif
