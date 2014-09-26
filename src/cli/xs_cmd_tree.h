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

#ifndef __XS_CMD_TREE_H__
#define __XS_CMD_TREE_H__

typedef struct xs_cmd_tree_node_t
{
    char*               reg; /* 命令或者正则表达式，用来判断命令或者tab补全 */
//    char*               pre;
    regex_t             regex;
    xs_tree_node_t      node;
    int                 end;
} xs_cmd_tree_node_t;

extern xs_tree_t g_cmd_tree;

void xs_cmd_reg(/*char* pre, */char* cmd);
void xs_cmd_clear();
char* xs_cmd_resolv(char* cmd, int* size);

#endif
