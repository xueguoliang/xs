
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
#ifndef __XS_HASH_H__
#define __XS_HASH_H__

typedef struct xs_hash_node_t
{
    union
    {
        const char*     s;
        intptr_t i;
    } key;
    void*           value;
    struct rb_node  node;
} xs_hash_node_t;

typedef struct xs_hash_t
{
    struct rb_root root;
} xs_hash_t;



static inline void xs_hash_node_destroy(xs_hash_node_t* node)
{
    xs_free(node);
}

static inline xs_hash_node_t* _xs_hash_node_create(const char* key, void* ptr, const char* file, int line)
{
    xs_hash_node_t* node = (xs_hash_node_t*)__xs_malloc(sizeof(*node), file, line);
    node->key.s = key;
    node->value = ptr;
    return node;
}

static inline xs_hash_node_t* _xs_hash_nodei_create(int key, void* ptr, const char* file, int line)
{
    xs_hash_node_t* node = (xs_hash_node_t*)__xs_malloc(sizeof(*node), file, line);
    node->key.i = key;
    node->value = ptr;
    return node;
}

static inline int _xs_hash_cmpi(const struct rb_node* pos, const void* ptr)
{
    xs_hash_node_t* node = xs_entry(pos, xs_hash_node_t, node);
    return node->key.i - (intptr_t)ptr;
}

static inline int _xs_hash_cmp(const struct rb_node* pos, const void* ptr)
{
    xs_hash_node_t* node = xs_entry(pos, xs_hash_node_t, node);
    return strcmp(node->key.s, (char*)ptr);
}

#define xs_hash_node_create(key, ptr) _xs_hash_node_create(key, ptr, __FILE__, __LINE__)
#define xs_hash_nodei_create(key, ptr) _xs_hash_nodei_create(key, ptr, __FILE__, __LINE__)

static inline xs_hash_t* _xs_hash_create(const char* file, int line)
{
    xs_hash_t* hash = (xs_hash_t*)__xs_malloc(sizeof(xs_hash_t), file, line);
    rb_init(&hash->root, _xs_hash_cmp);
    return hash;
}
static inline xs_hash_t* _xs_hashi_create(const char* file, int line)
{
    xs_hash_t* hash = (xs_hash_t*)__xs_malloc(sizeof(xs_hash_t), file, line);
    rb_init(&hash->root, _xs_hash_cmpi);
    return hash;
}

#define xs_hash_create() _xs_hash_create(__FILE__, __LINE__)
#define xs_hashi_create() _xs_hashi_create(__FILE__, __LINE__)

static inline void xs_hash_destroy(xs_hash_t* hash, void (*clear_func)(xs_hash_node_t*))
{
    xs_hash_node_t* node;
    while(!rb_empty(&hash->root))
    {
        node = xs_entry(hash->root.rb_node, xs_hash_node_t, node);
        rb_erase(&node->node, &hash->root);
        if(clear_func)
            clear_func(node);
        xs_hash_node_destroy(node);
    }
    xs_free(hash);
}

static inline int _xs_hash_add(xs_hash_t* hash, const char* key, void* ptr, const char* file, int line)
{
    xs_hash_node_t* node = _xs_hash_node_create(key, ptr, file, line);
    return rb_insert(&node->node, &hash->root, key);
}

#define xs_hash_add(hash, key, _hptr) _xs_hash_add(hash, (const char*)(intptr_t)key, _hptr, __FILE__, __LINE__)

#define xs_hash_del(hash, key, func) _xs_hash_del(hash, (char*)key, func)
static inline void _xs_hash_del(xs_hash_t* hash, char* key, void(*clear_func)(xs_hash_node_t*))
{
    xs_hash_node_t* node;
    struct rb_node* pos;
    
    pos = rb_find(&hash->root, key);
    if(!pos)
        return;

    node = xs_entry(pos, xs_hash_node_t, node);
    rb_erase(&node->node, &hash->root);

    if(clear_func)
        clear_func(node);
    else
        xs_hash_node_destroy(node);
}

#define xs_hash_find(hash, key) _xs_hash_find(hash, (void*)key)
static inline void* _xs_hash_find(xs_hash_t* hash, const void* key)
{
    xs_hash_node_t* node;
    struct rb_node* pos;

    if(hash == NULL)
        return NULL;
    
    pos = rb_find(&hash->root, key);
    if(!pos)
        return NULL;

    node = xs_entry(pos, xs_hash_node_t, node);
    return node->value;
}

static inline xs_hash_node_t* xs_hash_first(xs_hash_t* hash)
{
    struct rb_node* node = rb_first(&hash->root);
    if(node)
        return xs_entry(node, xs_hash_node_t, node);
    return NULL;
}

static inline xs_hash_node_t* xs_hash_next(xs_hash_node_t* prev)
{
    struct rb_node* node = rb_next(&prev->node);
    if(node)
        return xs_entry(node, xs_hash_node_t, node);
    return NULL;
}

#endif
#ifdef __cplusplus
}
#endif
