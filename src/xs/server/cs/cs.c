
#include "server.h"

void user_req_service(xs_model_t* model, int fd, struct xs_ctrl_t* ctrl)
{
    model = model;
    fd = fd;
    ctrl = ctrl;

    int fd1 = xs_sock_connect(IS_PORT, IS_IP);
    if(fd1 < 0)
    {
        rsp_error("IS down", fd);
        return;
    }

    int ret = xs_model_send_block(model, fd1, 60000);
    if(ret != 0)
    {
        rsp_error("send IS info failure", fd);
        close(fd1);
        return;
    }

    xs_model_t* rsp;
    ret = xs_model_recv_block(&rsp, fd1, 60000);
    if(ret != 0)
    {
        rsp_error("recv IS info failure", fd);
        close(fd1);
        return;
    }

    // rsp the bs to client
    xs_model_send_and_close(fd, rsp);
}

void server_init()
{
    xs_ctrl_t* ctrl = xs_ctrl_create(CS_PORT, NULL, NULL);
    xs_ctrl_reg_handler(ctrl, __xs_user, __xs_req_service, user_req_service);
}
