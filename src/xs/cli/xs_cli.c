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
#include "xs_cli.h"
/*
 * node add 1.1.1.1
 * */

void xs_cli_init(int port, char* ip)
{
    int fd;
    char* buf;
    int size;
    xs_model_t* rsp;
    xs_init();
    xs_cmd_reg("clis init");

    fd = xs_sock_connect(port, ip);
    buf = xs_cmd_resolv("clis init", &size);
    xs_logd("buf is (%s)", buf+4);
 //   xs_sock_send_block(fd, buf, size, 60000);
    xs_model_send_arg(fd, 2, __xs_clis, __xs_init);
    xs_model_recv_block(&rsp, fd, 60000);
    int i;
    for(i=0; i<rsp->argc; i++)
    {
        xs_logd("rsp %d is (%s)", i, rsp->argv[i]);
    }

    while(rsp->argc > 0)
    {
        --rsp->argc;
        xs_cmd_reg(rsp->argv[rsp->argc]);
        xs_free(rsp->argv[rsp->argc]);
    }
    xs_free(rsp);
    xs_close_socket(fd);
}

void xs_cli_exit()
{
    xs_cmd_clear();
    xs_fini();
}
