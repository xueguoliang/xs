
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
#include "xs.h"

/* ifdef debug */

#ifndef XS_AUTO_GEN_FILES
int g_mem_file_count = 0;
char* g_mem_files[1] = {NULL};
#endif

#define XS_MEM_MAGIC 0x5d05dd5d
#define XS_MEM_SET_TAIL_MAGIC(block, size) *(uint32_t*)((char*)block + (size-4)) = XS_MEM_MAGIC
#define XS_MEM_GET_TAIL_MAGIC(block, size) *(uint32_t*)((char*)block + (size-4))


/* table for block  */
    /* 32 64 128 256 512 1K 2K 4K 8K 16K 32K 64K 128K 256K 512K 
     * 0  1  2   3   4   5   6  7  8  9  10  11  12   13   14   
     * */

xs_mempool_t* xs_mempool_create(void* rtdata, int rtid, int max_alloc_size)
{
    int i;

    xs_mempool_t* mem_pool;
    mem_pool = malloc(sizeof(*mem_pool));

    mem_pool->rtid = rtid;
    mem_pool->rtdata = rtdata;
#ifdef XS_USE_MEMPOOL
    mem_pool->use_buff = 0;

    xs_list_init(&mem_pool->extra);

    max_alloc_size += XS_MEM_EXTRA;

    mem_pool->trunks = NULL;

    mem_pool->min_block_size = XS_MEMPOOL_MIN_BLOCK_SIZE;
    mem_pool->min_block_index = 0;

#if 1
    if(max_alloc_size < 64)
        max_alloc_size = 64;
    if(max_alloc_size > 512*1024)
        max_alloc_size = 512*1024;
#endif

    mem_pool->max_block_index = 0;
    while(max_alloc_size > mem_pool->min_block_size)
    {
        ++mem_pool->max_block_index;
        max_alloc_size /= 2;
    }
    mem_pool->max_block_size = (mem_pool->min_block_size << mem_pool->max_block_index);
    mem_pool->blocks = malloc(sizeof(*mem_pool->blocks) * (mem_pool->max_block_index+1));

    for(i=0; i<=mem_pool->max_block_index; ++i)
    {
        mem_pool->blocks[i] = NULL;
    }

    xs_logd("max block=%dK", mem_pool->max_block_size/1024);
#endif

    return mem_pool;
}

#ifdef XS_USE_MEMPOOL

int xs_mem_file_index(const char* file1)
{
    int b=0;
    int e=g_mem_file_count;
    int m;
    int ret;
    const char* file = file1;

    if(e == 0)
        return -1;

    /* if filename is start with ./, clear it */
    if(*file1 == '.')
        file = file1 + 2;

    while(b<=e)
    {
        m = (b+e)/2;
        ret = strcmp(file, g_mem_files[m]);
        if(ret == 0)
            return m;
        if(ret > 0)
            b = m+1;
        else 
            e = m-1;
    }

    return -1;
}


void xs_mempool_check_mem_leak(xs_mempool_t* mem_pool)
{
    xs_mempool_trunk_t* p = mem_pool->trunks;
    xs_mempool_block_t* b;
    int size;
    int bsize;
    char buf[256];
    FILE* f ; 

    /* get filename and unlink */
    sprintf(buf, "mem_leak_%02d.txt", mem_pool->rtid);
    unlink(buf);

    f = fopen(buf, "w+");
   
    if(f)
       fprintf(f, "use_buff=%d\n", mem_pool->use_buff); 

    while(p)
    {
        size = 0;
        while(p->size > size)
        {
            b = (xs_mempool_block_t*)(p->buf + size);
            bsize = mem_pool->min_block_size << b->index;
            if(b->file != (uint16_t)-1)
            {
                if(f)
                    fprintf(f, "filename=%s line=%d block_size=%d\n", g_mem_files[b->file], b->line, bsize);
            }
            size += bsize;
        }
        p = p->next;
    }

    /* check extra */
    xs_list_t* node = mem_pool->extra.next;
    while(node != &mem_pool->extra)
    {
        b = (xs_mempool_block_t*)((char*)node + sizeof(xs_list_t));
        uint16_t __index = b->index;
        if(__index != (uint16_t)-1)
        {
            if(f)
                fprintf(f, "filename=%s line=%d\n", g_mem_files[b->file], b->line);
        }
        else
        {
            if(f)
                fprintf(f, "filename=%s line=%d\n", g_mem_files[b->file], b->line);
        }

        node = node->next;
    }

    if(f)
        fclose(f);
}

void xs_mempool_destroy(xs_mempool_t* mem_pool)
{
    int size = 0;
    int block_size;
    xs_mempool_block_t* block;

    while(mem_pool->trunks)
    {
        xs_mempool_trunk_t* p = mem_pool->trunks;
        mem_pool->trunks = mem_pool->trunks->next;
        /* TODO: check mem trunk if memleak first */
        size = 0;
        while(p->size > size)
        {
        	block = (xs_mempool_block_t*)(p->buf + size);
            block_size = mem_pool->min_block_size << block->index;
        	if(block->file != (uint16_t)-1)
            {
               // xs_error("mem leak, filename=%s, line=%d, block_size=%d", g_mem_files[block->file], block->line, block_size);
            }
            size += block_size;
        }
        /* free trunk memory */
        free(p);
    }

    /* check extra */
    while(!xs_list_empty(&mem_pool->extra))
    {
        xs_list_t* node = mem_pool->extra.next;
        xs_list_del_head(&mem_pool->extra);
        block = (xs_mempool_block_t*)((char*)node + sizeof(xs_list_t));
        uint16_t __index = block->index;
        if(__index != (uint16_t)-1)
        {
         //   xs_error("*******mem leak, filename=%s, line=%d", g_mem_files[block->file], block->line);
        }
        else
        {
          //  xs_error("********mem leak, filename=unknown, line=%d", block->line);
        }
        free(node);
    }

    /* init blocks */
    int i;
    for(i=0; i<mem_pool->max_block_index; ++i)
    {
        mem_pool->blocks[i] = NULL;
    }
}

xs_mempool_block_t* __xs_mempool_alloc_block(xs_mempool_t* mem_pool, int index, int size)
{
    int which = index;
    xs_mempool_trunk_t* trunk = NULL;
    xs_mempool_block_t* block = NULL;


    while(mem_pool->blocks[index] == NULL)
    {
        if(index == mem_pool->max_block_index)
            break;

        size <<= 1;
        ++index;
    }

    if(mem_pool->blocks[index] == NULL)
    {
        trunk = malloc(sizeof(*trunk) + mem_pool->max_block_size);
        trunk->next = mem_pool->trunks;
        mem_pool->trunks = trunk;

        trunk->size = mem_pool->max_block_size;

        block = (xs_mempool_block_t*)trunk->buf;
        block->index = mem_pool->max_block_index;
        block->file = -1;

        block->magic = (uint16_t)XS_MEM_MAGIC;
        XS_MEM_SET_TAIL_MAGIC(block, mem_pool->max_block_size);
    }
    else
    {
        block = mem_pool->blocks[index];
        mem_pool->blocks[index] = mem_pool->blocks[index]->next;
    }

    while(which != index)
    {
         size >>= 1;
         xs_mempool_block_t* half = (xs_mempool_block_t*)((char*)block + size);
         --index;

         half->index = index;
         half->file = -1;
         half->magic = (uint16_t)XS_MEM_MAGIC;
         XS_MEM_SET_TAIL_MAGIC(half, size);

         half->next = mem_pool->blocks[index];
         mem_pool->blocks[index] = half->next;

         block->index = index;
         XS_MEM_SET_TAIL_MAGIC(block, size);
    }

    mem_pool->use_buff += size;

    return block;
}

void* xs_mempool_alloc(xs_mempool_t* mem_pool, int size, const char* file, int line, uint16_t thid)
{
    xs_mempool_block_t * block;
    int size_fit = mem_pool->min_block_size;
    int index = 0;


    size += XS_MEM_EXTRA;
    if(size > mem_pool->max_block_size)
    {
        size += sizeof(xs_list_t);
        char* buf = malloc(size);
        block = (xs_mempool_block_t*)(buf+sizeof(xs_list_t));
        block->index = 0xf;
        block->magic = (uint16_t)XS_MEM_MAGIC;
        block->file = xs_mem_file_index(file);
        block->line = line;
        xs_list_t* node = (xs_list_t*)buf;
        xs_list_init(node);
        xs_list_add_tail(&mem_pool->extra, node);
        return &block->next;
    }

    for(index=0; ;++index)
    {
        if(size_fit >= size)
            break;
        size_fit <<= 1;
    }

    block = __xs_mempool_alloc_block(mem_pool, index, size_fit);
    block->index = index;

    /* TODO record files */
    block->file = xs_mem_file_index(file);
    block->line = line;

    block->magic = (uint16_t)XS_MEM_MAGIC;
    XS_MEM_SET_TAIL_MAGIC(block, size_fit);
    block->thid = thid;

    return &block->next;
}

void xs_mempool_check_magic(void* ptr)
{
    xs_mempool_t* mem_pool;
    xs_mempool_block_t* block = (xs_mempool_block_t*)((char*)ptr - XS_MEM_HEAD);
    int thid = block->thid;
    if(thid == 0)
    {
        mem_pool = g_process.mp;
    }
    else
    {
        xs_logd("check thread memory magic");
        mem_pool = g_process.thre[thid]->mp;
    }
    
    int size;
    // TODO check magic here
    if(block->magic != (uint16_t)XS_MEM_MAGIC)
    {
        xs_loge("check magic now ptr=%p, block->thid=%d, block->magic=%hx", 
                ptr, (int)block->thid, (uint16_t)block->magic);
        //TODO how to check
        {
            size = 0;
            while(size < mem_pool->max_block_size)
            {
                block = (xs_mempool_block_t*)((char*)block - mem_pool->min_block_size);
                size += mem_pool->min_block_size;

                if(block->magic == (uint16_t)XS_MEM_MAGIC)
                {
                    if(block->file < g_mem_file_count)
                        xs_loge("file=%s, line=%d", g_mem_files[block->file], block->line);
                    else
                        xs_loge("unknown file, line=%d", block->line);
                    break;
                }
            }
        }
        exit(1);
    }

    if(block->index == 0xf)
    {
        return;
    }

    size = mem_pool->min_block_size << block->index;
    if(XS_MEM_GET_TAIL_MAGIC(block, size) != XS_MEM_MAGIC)
    {

        xs_loge("magic err 0x%x", XS_MEM_GET_TAIL_MAGIC(block, size));
        xs_loge("size=%d", size);
        xs_loge("ptr=%p", ptr);

        if(block->file < g_mem_file_count)
            xs_loge("file=%s, line=%d", g_mem_files[block->file], block->line);
        else
            xs_loge("unknown file, line=%d", block->line);

        exit(1);
    }

}

void xs_mempool_free(xs_mempool_t* mem_pool, void* ptr)
{
    xs_mempool_block_t* block = (xs_mempool_block_t*)((char*)ptr - XS_MEM_HEAD);
    int size;
    
    // TODO check magic here
    if(block->magic != (uint16_t)XS_MEM_MAGIC)
    {
        xs_loge("check magic now ptr=%p, block->thid=%d, block->magic=%hx", 
                ptr, (int)block->thid, (uint16_t)block->magic);
		if(block->file < g_mem_file_count)
			xs_loge("file=%s, line=%d", g_mem_files[block->file], block->line);
		else
			xs_loge("file=%s, line=%d", "not know", block->line);
        //TODO how to check
        {
            size = 0;
            while(size < mem_pool->max_block_size)
            {
                block = (xs_mempool_block_t*)((char*)block - mem_pool->min_block_size);
                size += mem_pool->min_block_size;

                if(block->magic == (uint16_t)XS_MEM_MAGIC)
                {
                    if(block->file < g_mem_file_count)
                        xs_loge("file=%s, line=%d", g_mem_files[block->file], block->line);
                    else
                        xs_loge("unknown file, line=%d", block->line);
                    break;
                }
            }
        }
        exit(1);
    }

    if(block->index == 0xf)
    {
        xs_list_t* node = (xs_list_t*)((char*)block - sizeof(xs_list_t));
        xs_list_del(node);
        free(node);
        return;
    }

    size = mem_pool->min_block_size << block->index;
    mem_pool->use_buff -= size;
    if(XS_MEM_GET_TAIL_MAGIC(block, size) != XS_MEM_MAGIC)
    {

        xs_loge("magic err 0x%x", XS_MEM_GET_TAIL_MAGIC(block, size));
        xs_loge("size=%d", size);
        xs_loge("ptr=%p", ptr);

        if(block->file < g_mem_file_count)
            xs_loge("file=%s, line=%d", g_mem_files[block->file], block->line);
        else
            xs_loge("unknown file, line=%d", block->line);

        exit(1);
    }

    block->file = -1;
    block->next = mem_pool->blocks[block->index];
    mem_pool->blocks[block->index] = block;
}
#endif

#ifdef __cplusplus
}
#endif
