
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
#ifndef __XS_DICT_H__
#define __XS_DICT_H__

typedef struct xs_dict_pair_t
{
    char*           key;
    char*           value;
    struct rb_node  node;
} xs_dict_pair_t;

typedef struct xs_dict_t
{
    struct rb_root  root;
    int             size;
} xs_dict_t;

static inline int xs_dict_compare(const struct rb_node* node, const void* key)
{
    xs_dict_pair_t* pair = rb_entry(node, xs_dict_pair_t, node);
    return strcmp(pair->key, (char*)key);
}

static inline xs_dict_t* xs_dict_create2(const char* file, int line)
{
    xs_dict_t* dict =  (xs_dict_t*)__xs_zalloc(sizeof(xs_dict_t), file, line);
    rb_init(&dict->root, xs_dict_compare);
    return dict;
}

#define xs_dict_create() xs_dict_create2(__FILE__, __LINE__)

static inline void xs_dict_pair_destroy(xs_dict_pair_t* pair)
{
    xs_free(pair->key);
    xs_free(pair->value);
    xs_free(pair);
}

static inline xs_dict_pair_t* xs_dict_pair_create2(const char* key, const char* value, const char* file, int line)
{
    xs_dict_pair_t* pair = (xs_dict_pair_t*)__xs_malloc(sizeof(xs_dict_pair_t), file, line);
    pair->key = __xs_strdup(key, file, line);
    pair->value = __xs_strdup(value, file, line);
    return pair;
}

#define xs_dict_pair_create(key, value) xs_dict_pair_create2(key, value, __FILE__, __LINE__)

static inline void xs_dict_destroy(xs_dict_t* dict)
{
    xs_dict_pair_t* pair;

    if(dict == NULL)
        return;

    while(!RB_EMPTY_ROOT(&dict->root))
    {
        pair = rb_entry(dict->root.rb_node, xs_dict_pair_t, node);
        rb_erase(&pair->node, &dict->root); 

        xs_dict_pair_destroy(pair);
    }

    xs_free(dict);
}

static inline xs_dict_pair_t* xs_dict_find_pair(xs_dict_t* dict, const char* key)
{
    struct rb_node* node = rb_find(&dict->root, key);
    if(node == NULL)
        return NULL;
    return rb_entry(node, xs_dict_pair_t, node);
}


static inline void xs_dict_remove(xs_dict_t* dict, const char* key)
{
    xs_dict_pair_t* pair = xs_dict_find_pair(dict, key);
    if(pair == NULL)
        return;
    
    rb_erase(&pair->node, &dict->root);
    dict->size -= strlen(pair->key);
    dict->size -= strlen(pair->value);
    dict->size -= 2;

    xs_dict_pair_destroy(pair);
}



static inline xs_dict_t* xs_dict_add_pair1(xs_dict_t* dict, xs_dict_pair_t* pair, const char* file, int line)
{
    if(dict == NULL)
        dict = xs_dict_create2(file, line);

    if(rb_insert(&pair->node, &dict->root, pair->key) < 0)
    {
        xs_dict_remove(dict, pair->key);
        rb_insert(&pair->node, &dict->root, pair->key);
    }
    
    dict->size += strlen(pair->key);
    dict->size += strlen(pair->value);
    dict->size += 2;

    return dict;
}

static inline xs_dict_t* xs_dict_add_str1(xs_dict_t* dict, const char* key, const char* value, const char* file, int line)
{
    if(!value)
        return dict;
    xs_dict_pair_t* pair = xs_dict_pair_create2(key, value, file, line);
    return xs_dict_add_pair1(dict, pair, file, line);
}

#define xs_dict_add_pair(dict, pair) xs_dict_add_pair1(dict, pair, __FILE__, __LINE__)
#define xs_dict_add_str(dict, key, value) xs_dict_add_str1(dict, key, value, __FILE__, __LINE__)

static inline xs_dict_t* xs_dict_add_int1(xs_dict_t* dict, char* key, int value, const char* file, int line)
{
    char buf[32];
    xs_itoa_r(value, buf);
    
    return xs_dict_add_str1(dict, key, buf, file, line);    
}
#define xs_dict_add_int(dict, key, value) xs_dict_add_int1(dict, key, value, __FILE__, __LINE__)

static inline char* xs_dict_find_value(xs_dict_t* dict, const char* key)
{
    if(dict == NULL)
        return NULL;

    xs_dict_pair_t* pair = xs_dict_find_pair(dict, key);
    if(pair == NULL)
        return NULL;
    return pair->value;
}

static inline int xs_dict_compare_key(xs_dict_t* dict1, xs_dict_t* dict2, char* key)
{
    char* value1 = xs_dict_find_value(dict1, key);
    char* value2 = xs_dict_find_value(dict2, key);
    return strcmp(value1, value2);
}

static inline char* xs_dict_to_buf_with_head2(xs_dict_t* dict,
        int *size, char sp1, char sp2, const char* file, int line)
{
    char* buf = (char*)__xs_malloc(dict->size + 5, file, line);
    *size = dict->size;

    *(int*)buf = htonl(*size);
    *size += 4;

    int ret = 0;
    xs_dict_pair_t* pair;
    while(!RB_EMPTY_ROOT(&dict->root))
    {
        //pair = rb_entry(dict->root.rb_node, xs_dict_pair_t, node);
        pair = rb_entry(rb_first(&dict->root), xs_dict_pair_t, node);
        ret += sprintf(buf+ret+4, "%s%c%s%c", pair->key, sp1, pair->value, sp2);
        xs_dict_remove(dict, pair->key);
    } 

    xs_dict_destroy(dict);

    return buf;
}

static inline char* xs_dict_to_buf1(xs_dict_t* dict, int* size, char sp1, char sp2, const char* file, int line)
{
    if(dict == NULL)
        return NULL;

    char* buf = (char*)__xs_malloc(dict->size + 1, file, line);
    if(size)
        *size = dict->size;

    int ret = 0;
    xs_dict_pair_t* pair;
    while(!RB_EMPTY_ROOT(&dict->root))
    {
        pair = rb_entry(dict->root.rb_node, xs_dict_pair_t, node);
        ret += sprintf(buf+ret, "%s%c%s%c", pair->key, sp1, pair->value, sp2);

        xs_dict_remove(dict, pair->key);
    }

    xs_dict_destroy(dict);
    return buf;
}

#define xs_dict_to_buf(__dict, __size) xs_dict_to_buf1(__dict, __size, '`', '`', __FILE__, __LINE__)
#define xs_dict_to_buf_s(__dict, __size, __sp1, __sp2) xs_dict_to_buf1(__dict, __size, __sp1, __sp2, __FILE__, __LINE__)
#define xs_dict_to_buf_with_head(__dict, __size) xs_dict_to_buf_with_head2(__dict, __size, '`', '`', __FILE__, __LINE__)

static inline xs_dict_t* xs_dict_from_buf2(char* buf, const char* split, xs_dict_t* dict, const char* file, int line)
{
    char* saveptr = NULL;
    if(split == NULL)
        split = "`";

    char* key = strtok_r(buf, split, &saveptr);
    char* value;
    while(key)
    {
        value = strtok_r(NULL, split, &saveptr);
        if(value == NULL)
            break;

        dict = xs_dict_add_str1(dict, key, value, file, line);
        key = strtok_r(NULL, split, &saveptr);
    }

    return dict;
}
#define xs_dict_from_buf(__buf, __dict) xs_dict_from_buf2(__buf, "`", __dict, __FILE__, __LINE__)
#define xs_dict_from_buf_s(__buf, __dict, __split) xs_dict_from_buf2(__buf, __split, __dict, __FILE__, __LINE__)

static inline xs_dict_t* xs_dict_clone(xs_dict_t* dict)
{
    xs_dict_t* dest = NULL;
    if(dict->root.rb_node == NULL)
        return xs_dict_create();

    struct rb_node* node = rb_first(&dict->root);
    while(node)
    {
        xs_dict_pair_t* pair = rb_entry(node, xs_dict_pair_t, node);
        dest = xs_dict_add_str(dest, pair->key, pair->value);
        node = rb_next(node);
    }

    return dest;
}

static inline char* _xs_dict_dump2(xs_dict_t* dict, const char* file, int line)
{
    xs_dict_t* dict1 = xs_dict_clone(dict);
    char* buf = xs_dict_to_buf1(dict1, NULL, '=', ' ', file, line);
    return buf;
}

#define _xs_dict_dump(dict) _xs_dict_dump2(dict, __FILE__, __LINE__)

#define xs_dict_dump(__dict) do{\
    char* __xxbuf = _xs_dict_dump(__dict);\
    xs_info("dict dump(%s)", __xxbuf);\
    xs_free(__xxbuf);\
}while(0)

#define xs_dict_int(dict, key) \
    atoi(xs_dict_find_value(dict, key))
#define xs_dict_str(dict, key) \
    xs_strdup(xs_dict_find_value(dict, key))

#endif
#ifdef __cplusplus
}
#endif
