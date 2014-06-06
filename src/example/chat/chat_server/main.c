
#include "xs.h"
#include "../cc_model.h"

void xs_chat_wait_close(xs_ev_sock_t* sock)
{
    xs_logd("socket %d is closed", sock->fd);
    close(sock->fd);
    xs_free(sock->ev);
}

void xs_chat_model_send_done(xs_model_t* model, void* ptr, int result)
{
    model = model;
    ptr = ptr;
    result = result;
    if(result == XS_AIO_DONE)
        xs_ev_add_sock_ev((int)(intptr_t)ptr, EPOLLIN, xs_chat_wait_close, NULL);
    else
        close((int)(intptr_t)ptr);
}

void xs_chat_model_recv_done(xs_model_t* model, void* ptr, int result)
{
    // recv failure, close socket
    if(result != XS_AIO_DONE)
    {
        close((int)(intptr_t)ptr);
        return;
    }

    // handle login
    if(ccm_isLogin(model))
    {
        ccm_login* cc_login = (ccm_login*)model;
        xs_logd("username(%s), password(%s), want login", cc_login->username, cc_login->password); 

        int fd = (int)(intptr_t)ptr;
        xs_model_t* model = xs_model_create(2, __xs_ok, __xs_ok);
        xs_model_send(fd, model, xs_chat_model_send_done, (void*)(intptr_t)fd);
    }
}


void xs_chat_accept(xs_ev_sock_t* sock)
{
    for(;;)
    {
        int fd = accept(sock->fd, NULL, NULL);
        if(fd < 0) break;

        xs_model_recv(fd, xs_chat_model_recv_done, (void*)(intptr_t)fd);
    } 
}

int main(int argc, char** argv)
{
    xs_server_init(6, NULL, argc, argv);
    xs_server_start_tcp(_PORT_DEFAULT, 10, xs_chat_accept, NULL);
    return xs_server_run();
}
