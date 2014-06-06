
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

typedef struct xs_model_t
{
    int             argc;
    char*           argv[];
} xs_model_t;

#define xs_model_argc(__type1) ((sizeof(__type1)-sizeof(int))/sizeof(char*))
#define xs_model(__type) (__type*)xs_model_alloc(xs_model_argc(__type))

#define argv(n) argv[n]
#define xs_model_set(_m, _i, _v) (_m)->argv[_i] = xs_strdup(_v)

#define __xs_argc "argc"
#define __xs_ok "ok"
#define __xs_err "err"
#define xs_success(buf) ((buf) && strcmp((buf), __xs_ok) == 0)

void* xs_model_alloc(int argc);
void xs_model_delete(xs_model_t* model);
xs_model_t* xs_model_create(int argc, ...);

xs_model_t* xs_model_from_buf(char* buf, int size);
char* xs_model_to_buf(xs_model_t* model, int* size);
static inline char* xs_model_to_buf_and_free_model(xs_model_t* model, int*size)
{
    char* buf = xs_model_to_buf(model, size);
    xs_model_delete(model);
    return buf;
}

/* this function, so danger */
static inline void xs_model_set_all(void* model, ...)
{
    int i;
    va_list ap;
    char* p;
    xs_model_t* m = (xs_model_t*)model;

    va_start(ap, model);

    for(i=0; i<m->argc; i++)
    {
        p = va_arg(ap, char*);
        if(p)
            m->argv[i] = xs_strdup(p);
    }
    va_end(ap);
}


/* for client */
int xs_model_call(int fd, xs_model_t** r, int argc, ...);
int xs_model_send_block(xs_model_t* model, int fd, int timeout);
int xs_model_recv_block(xs_model_t** model, int fd, int timeout);

static inline xs_model_t* xs_model_createv(int argc, va_list ap)
{
    int i;
    xs_model_t* m = (xs_model_t*)xs_model_alloc(argc);
    for(i=0; i<argc; i++)
    {
        m->argv[i] = xs_strdup(va_arg(ap, char*));
    }
    return m;
}

/* model send and recv */
typedef void(*model_callback_func_t)(xs_model_t*, void*, int);
typedef struct _xs_model_cb
{
    xs_model_t* model;
    model_callback_func_t callback_func;
    void* ptr;
} xs_model_cb;
void xs_model_recv(int fd, model_callback_func_t func, void* ptr);
void xs_model_send(int fd, xs_model_t* model, model_callback_func_t func, void* ptr);


#endif
#ifdef __cplusplus
}
#endif
