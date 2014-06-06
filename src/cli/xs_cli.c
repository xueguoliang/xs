
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
    xs_sock_send_block(fd, buf, size, XS_CLIS_TIMEOUT);
    xs_model_recv_block((void**)&rsp, fd, XS_CLIS_TIMEOUT);
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
