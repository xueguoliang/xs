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

char g_cmd[2048];

int main(int argc, char* argv[])
{
    int size;
    char* buf;
    int fd;
    char* ip = "127.0.0.1";
    xs_model_t* rsp = NULL;
    int port = CLI_SERVER_PORT;
    if(argc > 1)
    {
        port = atoi(argv[1]);
    }
    if(argc > 2)
    {
        ip = argv[2];
    }

    printf("init\n");
    xs_cli_init(port, ip);
    

    while(1)
    {
        printf("xsfs> ");

        fgets(g_cmd, sizeof(g_cmd), stdin);
        if( strncmp(g_cmd, "quit", 4) == 0 )
            break;
        if( *g_cmd == '\n' )
            continue;

        buf = xs_cmd_resolv(g_cmd, &size);

        if(buf)
        {
            fd = xs_sock_connect(port, ip);
            xs_aio_send_block(fd, buf, size, 60000);
            xs_free(buf);
            xs_model_recv_block(&rsp, fd, 60000);
            xs_close_socket(fd);

            int i;
            for(i=0; i<rsp->argc; i++)
                printf("%s\n", rsp->argv[i]);
            xs_model_delete(rsp);
        }
    }

    xs_cli_exit();
    return 0;
}

