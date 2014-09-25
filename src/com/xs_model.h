
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
#ifndef __XS_MODEL_H__
#define __XS_MODEL_H__

/*
 * model do not close socket too, same with aio
 * */

typedef struct xs_model_t
{
    intptr_t        argc;
    char*           argv[];
} xs_model_t;

static inline void xs_model_set(xs_model_t* model, int i, const char* arg)
{
    model->argv[i] = xs_strdup(arg);
}

#define __xs_ok "ok"
#define __xs_err "err"
#define xs_success(__result) __result && strcmp(__result, __xs_ok) == 0
#define argv(__n) argv[__n]

typedef struct xs_model_cb_t
{
    xs_model_t* model;
    xs_aio_t* aio;
    void* ptr;
    void(*func)(struct xs_model_cb_t*);
} xs_model_cb_t;

typedef void(*xs_model_cbk_t)(xs_model_cb_t* cb);

xs_model_t* xs_model_from_buf(char* buf);
char* xs_model_to_buf(xs_model_t* model, int* olen);

void xs_model_recv(int fd, xs_model_cbk_t cbk, void* ptr);
void xs_model_send(int fd, xs_model_cbk_t cbk, void* ptr, xs_model_t* model);
void xs_model_send_and_close(int fd, xs_model_t* model);
static inline xs_model_t* xs_model_create(int argc)
{
    xs_model_t* model = (xs_model_t*)xs_malloc(sizeof(model->argc)+sizeof(char*)*argc);
    memset(model, 0, sizeof(int)+sizeof(char*)*argc);
    model->argc = argc;
    return model;
}

static inline xs_model_t* xs_model_create_v(int argc, ...)
{
    int i;
    xs_model_t* model = xs_model_create(argc);
    va_list ap;
    va_start(ap, argc);
    for(i=0; i<argc; ++i)
    {
        model->argv[i] = xs_strdup(va_arg(ap, char*));
    }
    va_end(ap);
    return model;
}

static inline xs_model_t* xs_model_create_ap(int argc, va_list ap)
{
    int i;
    xs_model_t* model = xs_model_create(argc);
    for(i=0; i<argc; ++i)
    {
        model->argv[i] = xs_strdup(va_arg(ap, char*));
    }
    return model;
}

static inline void xs_model_delete(xs_model_t* model)
{
    int i;
    for(i=0 ;i<model->argc; ++i)
    {
        xs_free(model->argv[i]);
    }
    xs_free(model);
}

static inline void xs_model_send_and_close_v(int fd, int argc, ...)
{
    va_list ap;
    va_start(ap, argc);
    xs_model_t* model = xs_model_create_ap(argc, ap);
    va_end(ap);
    xs_model_send_and_close(fd, model);
}

xs_model_t* xs_model_clone(xs_model_t* model);

#endif
#ifdef __cplusplus
}
#endif
