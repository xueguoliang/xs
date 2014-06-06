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

void xs_tree_insert(xs_tree_t* tree, xs_tree_node_t* parent, xs_tree_node_t* node, const void* key)
{
    node->parent = parent;
    if(parent == NULL)
    {
        tree->root = node;
    }
    else  
    {
        rb_insert(&node->node, &parent->child, key);
    }
}

void xs_tree_remove(xs_tree_t* tree, xs_tree_node_t* node)
{
    if(node->parent == NULL)
        tree->root = NULL;
    else
        rb_erase(&node->node, &node->parent->child);
}

xs_tree_node_t* xs_tree_find(xs_tree_node_t* parent,
        void* key)
{
    struct rb_node* node;
    node = rb_find(&parent->child, key);
    if(node)
        return xs_entry(node, xs_tree_node_t, node);
    return NULL;
}
#ifdef __cplusplus
}
#endif
