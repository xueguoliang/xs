
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

#ifndef XS_USE_MEMPOOL
void* __xs_malloc(int size, const char *file, int line)
{
    file = file;line=line;
    return malloc(size);
}
void* __xs_zalloc(int size, const char *file, int line)
{
    void* ptr = malloc(size);
    memset(ptr, 0, size);
    file = file;line=line;
    return ptr;
}
char* __xs_strdup(const char *ss, const char *file, int line)
{
    file = file;line=line;
    return strdup(ss);
}
void __xs_free(void *ptr)
{
    return free(ptr);
}
void* __xs_realloc(void* ptr, int size, const char* file, int line)
{
    file = file;line=line;
    return realloc(ptr, size);
}

#else
void* __xs_malloc(int size, const char* file, int line)
{
    xs_mempool_t* mp = pthread_getspecific(g_thread_key);
    return xs_mempool_alloc(mp, size, file, line, mp->rtid);
}

void* __xs_zalloc(int size, const char* file, int line)
{
    void* ptr = __xs_malloc(size, file, line);
    memset(ptr, 0, size);
    return ptr;
}

#if 0
void* __xs_realloc(void *ptr, int size, const char *file, int line)
{
#if 0
    xs_mempool_block_t* b = xs_mempool_get_block(ptr);
    int size1 = xs_mempool_get_block_size(b) - XS_MEM_EXTRA;
    if(size1 >= size)
    {
        return ptr;
    }
#endif

    void* ptrNew = __xs_malloc(size, file, line);
    memcpy(ptrNew, ptr, size);
    xs_free(ptr);
    return ptrNew;
}
#endif

char* __xs_strdup(const char* ss, const char* file, int line)
{
    if(ss == NULL)
        return NULL;
    int size = strlen(ss);
    char* p = __xs_malloc(size+1, file, line);
    strcpy(p, ss);
    return p;
}

void __xs_free(void* ptr)
{
    if(ptr == NULL)
        return;

    xs_mempool_t* mp = pthread_getspecific(g_thread_key);
    xs_mempool_block_t* b = xs_mempool_get_block(ptr);
    xs_thread_t* th;

    if(b->thid > g_process.thre_count)
    {
        //xs_loge("critical error mem block, file=%s, line=%d", file, line);
        exit(1);
    }

    if(mp->rtid == b->thid)
    {
        xs_mempool_free(mp, ptr);
    }
    else
    {
        xs_ev_t* ev = (xs_ev_t*)ptr;
        XS_EV_INIT(ev, XS_EV_FREE);

        if(mp->rtid == 0)
        {
            /* main thread */
            th = g_process.thre[b->thid];
            xs_list_add_tail(&th->ev_recv, &ev->node);
        }
        else
        {
            /* sub thread*/
            th = g_process.thre[mp->rtid];
            xs_list_add_tail(&th->ev_send, &ev->node);
        }
    }
}
#endif
#ifdef __cplusplus
}
#endif
