
#include "xs_recv.h"
#include "xs_db.h"

void xs_server_recv(xs_recv_ctrl_t* rc)
{
    char buf[1024];
    int ret = recv(rc->fd, buf, sizeof(buf), 0);
    if(ret > 0)
    {
        xs_db_add_record(buf, ret);
        send(rc->fd, buf, strlen(buf)+1, 0);
    }
    else
    {
        xs_close_socket(rc->fd);
        xs_free(rc);
    }
}

xs_recv_ctrl_t* xs_get_ctrl(int fd)
{
    xs_recv_ctrl_t* ctrl = xs_malloc(sizeof(*ctrl));
    ctrl->fd = fd;
    return ctrl;
}
