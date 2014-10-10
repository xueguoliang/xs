
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
/*
 * Copyright 2012-2015 Xueguoliang(xueguoliang@gmail.com)
 * */
#ifdef __cplusplus
extern "C"{
#endif
#ifndef __XS_MEMPOOL_H__
#define __XS_MEMPOOL_H__

// 内存块头部偏移字节数
#define XS_MEM_HEAD 8
// 内存块尾部预留字节数
#define XS_MEM_TAIL 4
// 最小内存块尺寸
#define XS_MEMPOOL_MIN_BLOCK_SIZE 32
//
#define XS_MEM_EXTRA (XS_MEM_HEAD+XS_MEM_TAIL)

// 内存池开关宏
#define XS_USE_MEMPOOL

extern pthread_key_t g_thread_key;

// 描述一个内存块的结构体
typedef struct xs_mempool_block_t
{
#define XS_MEMPOOL_HOT  1
#define XS_MEMPOOL_COOL 0
    uint16_t        magic;  /* 站 */
    uint16_t        file;    /* file index */
    uint16_t        line;  /* who malloc this memory, index of the thread */
    uint16_t        thid:12;    /* thread id */
    uint16_t        index:4;    /* 0000--1111, if 1111, just free */
    
    struct xs_mempool_block_t* next;
} xs_mempool_block_t;
// 一个内存块
typedef struct xs_mempool_trunk_t
{
    struct xs_mempool_trunk_t* next;
    int             size; /* size is 64k 128k 256k 512k or 1M */
    char            buf[];
} xs_mempool_trunk_t;
// 一个内存池
typedef struct xs_mempool_t
{
#ifdef XS_USE_MEMPOOL
    xs_mempool_trunk_t* trunks;
    xs_mempool_block_t** blocks;
    int             min_block_index;  // 0
    int             max_block_index;  // 15
    int             min_block_size;  // 32
    int             max_block_size; // 512k
    xs_list_t       extra; /* some malloc > max_block_size */
    uint32_t        use_buff;  // 已经申请的内存，统计
#endif
    /* the thread of the mempool */
    int             rtid;               // 线程编号
    void*           rtdata;         // 线程结构体
} xs_mempool_t;

xs_mempool_t* xs_mempool_create(void* rtdata, int rtid, int max_alloc_size);
extern xs_mempool_t* g_mp;

#ifdef XS_USE_MEMPOOL
/* general by Makefile */
extern char* g_mem_files[];
extern int g_mem_file_count;
void xs_mempool_check_magic(void* ptr);

void xs_mempool_destroy(xs_mempool_t* mem_pool);

void* xs_mempool_alloc(xs_mempool_t* pool, int size, const char* file, int line, uint16_t thread_id);
void xs_mempool_free(xs_mempool_t* pool, void* ptr);
void xs_mempool_check_mem_leak(xs_mempool_t* mem_pool);

static inline uint32_t xs_mempool_get_block_size(xs_mempool_block_t* block)
{
    return XS_MEMPOOL_MIN_BLOCK_SIZE << block->index;
}

static inline xs_mempool_block_t* xs_mempool_get_block(void* ptr)
{
    return (xs_mempool_block_t*)((char*)ptr - XS_MEM_HEAD);
}

#define xs_mem_dump(ptr) do\
{\
    xs_mempool_block_t* block = (xs_mempool_block_t*)((char*)ptr - XS_MEM_HEAD);\
    xs_loge("file=%s, line=%d, block->index=%d", g_mem_files[block->file], block->line, block->index);\
}while(0)
#endif

#endif
#ifdef __cplusplus
}
#endif
