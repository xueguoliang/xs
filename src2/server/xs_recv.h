#ifndef __XS_RECV_H__
#define __XS_RECV_H__

#include "../com/xs.h"

typedef struct xs_recv_ctrl_t
{
    int fd;
} xs_recv_ctrl_t;

void xs_server_recv(xs_recv_ctrl_t* recv);
xs_recv_ctrl_t* xs_get_ctrl(int fd);

#endif
