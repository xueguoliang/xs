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

#include "xs.h"
#ifdef __cplusplus
extern "C"{
#endif

typedef struct xs_clis_t
{
    xs_ctrl_t*  ctrl;
    xs_vec_t*   cmds;
} xs_clis_t;

xs_clis_t g_clis;

void xs_clis_init_handler(xs_model_t* model, int fd, struct xs_ctrl_t* ctrl)
{
    assert(model->argc == 2);
    ctrl = ctrl;

    xs_logd("cli_init start");

    xs_model_t* rsp = xs_model_create(g_clis.cmds->count);
    int i;
    for(i=0; i<g_clis.cmds->count; ++i)
    {
        rsp->argv[i] = xs_strdup((const char*)xs_vec_get(g_clis.cmds, i));
    }
    xs_model_send_and_close(fd, rsp);

    xs_logd("cli_init leave");
}
void xs_clis_show_handler(xs_model_t* model, int fd, struct xs_ctrl_t* ctrl)
{
    xs_clis_init_handler(model, fd, ctrl);
}

 void xs_clis_mem_check_handler(xs_model_t* model, int fd, struct xs_ctrl_t* ctrl)
 {
     model = model;
     fd = fd;
     ctrl = ctrl;

     xs_ev_add_msg_ev(XS_EV_MSG_MEM_CHECK, 0, NULL, NULL);

     xs_model_send_and_close_v(fd, 1, __xs_ok);
 }

// typedef void (*xs_object_handler_t)(xs_model_t* model, int fd, struct xs_ctrl_t* ctrl);
void xs_clis_reg_command(const char* cmd, xs_object_handler_t op)
{
    char* p = xs_strdup(cmd);

    char* saveptr;
    char* p1 = strtok_r(p, " ", &saveptr);
    char* p2 = strtok_r(NULL, " ", &saveptr);
    xs_ctrl_reg_handler(g_clis.ctrl, xs_strdup(p1), xs_strdup(p2), op);
    xs_vec_add(g_clis.cmds, xs_strdup(cmd));

    xs_free(p);
}

void xs_clis_init()
{
    g_clis.ctrl = xs_ctrl_create(__XS_CLIS_PORT, NULL, NULL);
    g_clis.cmds = xs_vec_create(4);

    xs_clis_reg_command(__xsc_clis " " __xsc_init, xs_clis_init_handler);
    xs_clis_reg_command(__xsc_clis " " __xsc_show, xs_clis_show_handler);
    xs_clis_reg_command(__xsc_memory " " __xsc_check, xs_clis_mem_check_handler);
}

void xs_clis_fini()
{
    xs_ctrl_destroy(g_clis.ctrl);
    xs_vec_destroy(g_clis.cmds, xs_free);
}

#ifdef __cplusplus
} 
#endif
