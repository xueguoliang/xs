
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
#ifndef __XS_OBJECT_H__
#define __XS_OBJECT_H__

struct xs_ctrl_t;
typedef xs_hash_t xs_object_t;

typedef void (*xs_object_handler_t)(xs_model_t* model, int fd, struct xs_ctrl_t* ctrl);

static inline xs_object_t* xs_object_create()
{
    xs_hash_t* hash = xs_hash_create();
    return hash;
}

static inline void xs_object_delete(xs_object_t* obj)
{
    xs_hash_destroy(obj, NULL);
}

static inline int xs_object_add_op(xs_object_t* obj, const char* name, xs_object_handler_t op)
{
    return xs_hash_add(obj, name, (void*)op);
}

#endif
#ifdef __cplusplus
}
#endif
