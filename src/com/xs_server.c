
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

#if 0
xs_clis_t g_clis;

void xs_clis_op_clis_init(void* req, int fd, xs_ctrl_t* ctrl)
{
    xs_cli_model_t* model = req;
    xs_logd("model->object=%s, model->op=%s", model->object, model->op);
    assert(model->count == 2);
    ctrl = ctrl;
    xs_model_t* rsp = xs_model_alloc(g_clis.cmds_count);
    int i;
    char key[32];

    for(i=0; i<g_clis.cmds_count; ++i)
    {
        rsp->argv[i] = xs_dict_str(g_clis.cmds, xs_itoa_r(i, key));
    }

    xs_model_send_and_close(rsp, fd);
    xs_model_delete(rsp);
}

void xs_clis_op_clis_show(void* req, int fd, xs_ctrl_t* ctrl)
{
    xs_clis_op_clis_init(req, fd, ctrl);
}

void xs_clis_op_memory_check(void* req, int fd, xs_ctrl_t* ctrl)
{
    xs_cli_model_t* model = req;
    xs_logd("model->object=%s, model->op=%s", model->object, model->op);
    assert(model->count == 2);
    ctrl = ctrl;
    xs_ev_add_msg_ev(XS_EV_MSG_MEM_CHECK, 0, NULL, NULL);

    xs_model_t* rsp = xs_model_create(2, __xs_ok, NULL);
    xs_model_send_and_close(rsp, fd);
    xs_model_delete(rsp);
}

void xs_clis_start(int port)
{
    g_clis.ctrl = xs_ctrl_create(port, XS_CLIS_TIMEOUT, NULL, NULL);

    xs_clis_reg_handler(__xs_memory, __xs_check, xs_clis_op_memory_check);
    xs_clis_reg_handler(__xs_clis, __xs_init, xs_clis_op_clis_init);
    xs_clis_reg_handler(__xs_clis, __xs_show, xs_clis_op_clis_show);

    xs_clis_reg_command(__xs_clis " " __xs_init);
    xs_clis_reg_command(__xs_clis " " __xs_show);
    xs_clis_reg_command(__xs_memory " " __xs_check);
}

void xs_clis_init(int port)
{
    g_clis.cmds = xs_dict_create();
    g_clis.ctrl = NULL;
    g_clis.cmds_count = 0;

    xs_clis_start(port);
}

void xs_clis_exit()
{
    xs_dict_destroy(g_clis.cmds);
    if(g_clis.ctrl)
        xs_ctrl_destroy(g_clis.ctrl);
}

void xs_clis_reg_command(char* command)
{
    char buf[32];
    int pos = g_clis.cmds_count ++;
    xs_dict_add_str(g_clis.cmds, xs_itoa_r(pos, buf), command);
}
#endif

void xs_server_init(int thread_count, void(*quit)(), int argc, char* argv[])
{
#ifdef WIN32
    // do nothing
#else
    signal(SIGPIPE, SIG_IGN);
#endif
    argc = argc;
    argv = argv;
    if(quit == NULL) quit = xs_fini;

    xs_init();
    xs_ev_init(thread_count, quit);

#if 0
    if(argc == 1)
        xs_clis_init(9989);
    else
        xs_clis_init(atoi(argv[1]));
#endif
}

static inline void xs_server_set_reuse(int fd)
{
    int option = 1;

    if (setsockopt ( fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option) ) < 0)
    {
        xs_loge("set socket reuse errno=%d", errno);
        exit(0);
    }

}

xs_ev_sock_t* xs_server_start_tcp(uint16_t port, int buf, void(*func)(xs_ev_sock_t*), void* ptr)
{
    int ret = 0;
    XSOCKET fd;
    fd = socket(AF_INET, SOCK_STREAM, 0);

    xs_server_set_reuse(fd);

    ret = xs_sock_bind(fd, port, NULL);
    if(ret < 0)
    {
        xs_loge("bind socket error");
        exit(0);
    }
    xs_sock_listen(fd, buf);
    xs_logi("server listen %d, sock=%d", port, fd);
    return xs_ev_add_sock_ev(fd, EPOLLIN, func, ptr);
}

#ifdef __cplusplus
}
#endif
