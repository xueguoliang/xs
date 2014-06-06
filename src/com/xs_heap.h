
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
#ifndef __XS_HEAP_H__
#define __XS_HEAP_H__


typedef struct __xs_heap
{
    void** data;
    int a;      /* all data count */
    int n;      /* used data count */
    int (*cmp)(const void*, const void*); /* compare */
} xs_heap_t;

static inline xs_heap_t* xs_heap_create(int size, int(*cmp)(const void*, const void*))
{
    xs_heap_t* heap = (xs_heap_t*)xs_malloc(sizeof(xs_heap_t));
    heap->data = (void**)xs_malloc(sizeof(void*)*size);
    heap->a = size;
    heap->n = 0;
    heap->cmp = cmp;

    return heap;
}

static inline void xs_heap_destroy(xs_heap_t* heap)
{
    if(heap)
    {
        if(heap->data)
        {
            xs_free(heap->data);
        }
        xs_free(heap);
    }
}

static inline int xs_heap_adjust_del(xs_heap_t* heap, int i)
{
    while(1)
    {
        int l = i*2+1;
        int r = i*2+2;
        if(l >= heap->n)
            return i;

        if(r >= heap->n)
        {
            heap->data[i] = heap->data[l];
            return l;
        }

        int res = heap->cmp(heap->data[l], heap->data[r]);
        if(res <= 0)
        {
            heap->data[i] = heap->data[l];
            i = l;
        }
        else
        {
            heap->data[i] = heap->data[r];
            i = r;
        }
    }
    return -1;
}

static inline void xs_heap_adjust_add(xs_heap_t* heap, int i)
{
    int p;
    while(i>0)
    {
        p = (i-1)/2;
        int res = heap->cmp(heap->data[p], heap->data[i]);
        if(res <= 0)
            break;
        
        void* tmp = heap->data[p];
        heap->data[p] = heap->data[i];
        heap->data[i] = tmp;
        i = p;
    }
    return;
}

static inline int xs_heap_add(xs_heap_t* heap, void* value)
{
    if(heap->n == heap->a)
    {
        /* 扩大内存 */
        void** tmp = (void**)xs_malloc(sizeof(void*)*(heap->a + 32));
        memcpy(tmp, heap->data, sizeof(void*)*heap->a);
        xs_free(heap->data);
        heap->data = tmp; 
        heap->a += 32; 
    }

    heap->data[heap->n++] = value;
    xs_heap_adjust_add(heap, heap->n-1);
    return 0;
}

static inline void* xs_heap_del(xs_heap_t* heap, int index)
{
    if(index >= heap->n)
        return NULL;

    void* ret = heap->data[index];
    void* f = heap->data[--heap->n];

    if(ret == f)
        return ret;

    int hole = xs_heap_adjust_del(heap, index);
    heap->data[hole] = f;
    xs_heap_adjust_add(heap, hole);
    return ret;
}

static inline int xs_heap_empty(xs_heap_t* heap)
{
    return heap->n == 0;
}

#endif
#ifdef __cplusplus
}
#endif
