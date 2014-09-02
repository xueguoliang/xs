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

#include "server.h"
typedef struct is_bs_t
{
    xs_list_t node;
    model_bs2is_reg_t* info;
    int cur_user_count;
} is_bs_t;

typedef struct is_t
{
    xs_list_t       bs;
    xs_ctrl_t*      ctrl;
} is_t;

// all global var in is
static is_t g_is;

xs_list_t* is_get_bs_list()
{
    return &g_is.bs;
}

is_bs_t* is_find_bs(const char* name)
{
    xs_list_t* pos;
    xs_list_t* head =  is_get_bs_list();
    xs_list_for_each(pos, head)
    {
        is_bs_t* bs = xs_list_entry(pos, is_bs_t, node);
        if(strcmp(bs->info->name, name) == 0)
            return bs;
    }

    return NULL;
}

is_bs_t* is_create_bs(xs_model_t* model)
{
    is_bs_t* bs = xs_malloc(sizeof(*bs));
    bs->info = (model_bs2is_reg_t*)xs_model_clone(model);
    bs->cur_user_count = 0;
    xs_list_init(&bs->node);
    return bs;
}

void is_reset_bs(is_bs_t* bs, xs_model_t* model)
{
    xs_model_delete((xs_model_t*)bs->info);
    bs->cur_user_count = 0;
    bs->info = (model_bs2is_reg_t*)xs_model_clone(model);
}

// bs reg to is
void is_bs_reg(xs_model_t* model, int fd, struct xs_ctrl_t* ctrl)
{
    model_bs2is_reg_t* m = (model_bs2is_reg_t*)model;
    is_bs_t* bs = is_find_bs(m->name);
    if(bs == NULL)
    {
        bs = is_create_bs(model);
        xs_list_add_tail(is_get_bs_list(), &bs->node);
    }
    else
    {
        is_reset_bs(bs, model);
    }
    xs_rsp_ok(fd);
    ctrl = ctrl;
}

is_bs_t* is_get_one_bs(const char* service_type)
{
    xs_list_t* node;
    xs_list_t* head = is_get_bs_list();
    xs_list_for_each(node, head)
    {
        is_bs_t* bs = xs_list_entry(node, is_bs_t, node);
        if(bs->cur_user_count < atoi(bs->info->max_user))
            return bs;
    }
    // not use now
    service_type = service_type;
    return NULL;
}

void is_user_req_service(xs_model_t* model, int fd, struct xs_ctrl_t* ctrl)
{
    model_user_req_service* m = (model_user_req_service*)model;

    is_bs_t* bs = is_get_one_bs(m->service_type);
    if(bs)
    {
        xs_rsp(fd, 3, __xs_ok, bs->info->ip, bs->info->port);
    }
    else
    {
        xs_rsp(fd, 2, __xs_err, "server busy");
    }
    ctrl = ctrl;
}

void server_init()
{
    xs_list_init(is_get_bs_list());

    // bs register to is
    g_is.ctrl = xs_ctrl_create(IS_PORT, NULL, NULL);
    // client send the req service to server
    xs_ctrl_reg_handler(g_is.ctrl, __xs_bs2is, __xs_reg, is_bs_reg);
    xs_ctrl_reg_handler(g_is.ctrl, __xs_user, __xs_req_service, is_user_req_service);
}
