
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
#ifndef __XS_TREE_H__
#define __XS_TREE_H__


typedef struct xs_tree_node_t
{
    struct rb_root child;
    struct rb_node node;

    struct xs_tree_node_t* parent;
} xs_tree_node_t;

typedef struct xs_tree_t
{
    xs_tree_node_t*     root;
} xs_tree_t;

void xs_tree_insert(xs_tree_t* tree, xs_tree_node_t* parent, xs_tree_node_t* node, const void* key);
void xs_tree_remove(xs_tree_t* r, xs_tree_node_t* n);
xs_tree_node_t* xs_tree_find(xs_tree_node_t* parent, void* key);

#endif
#ifdef __cplusplus
}
#endif
