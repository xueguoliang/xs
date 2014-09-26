#include "crequest.h"

CRequest::CRequest()
{
}

xs_model_t* CRequest::rpc_call(int argc, ...)
{
    int fd = xs_sock_connect(__XS_CHAT_PORT, "127.0.0.1");
    int ret;

    va_list ap;
    va_start(ap, argc);
    xs_model_t* model = xs_model_create_ap(argc, ap);
    va_end(ap);

    ret = xs_model_send_block(model, fd, 60000);
    xs_model_delete(model);
    if(ret != 0)
    {
        xs_close_socket(fd);
        return NULL;
    }

    ret = xs_model_recv_block(&model, fd, 60000);
    if(ret != 0)
    {
        xs_close_socket(fd);
        return NULL;
    }

    xs_close_socket(fd);
    return model;
}
