

#include "xs.h"

#if 0
void aio_done(xs_aio_t* aio)
{
    if(aio->result == XS_AIO_DONE)
    {
        int i;
        xs_model_t* model = xs_model_from_buf(aio->buf, aio->buflen);
        for(i=0; i<model->argc; ++i)
        {
            xs_logd("model%d=%s\n", i, model->argv[i]);
        }
        
        xs_aio_recv(aio->fd, aio_done, NULL);  /* read more data */
    }
    else
    {
        close(aio->fd);
        xs_logd("*******************************failure aio recv");
    }

    xs_aio_free(aio);
}
#endif

void model_recv_cbk(xs_model_t* model, void* ptr, int result)
{
    int i;
    ptr = ptr;
    result = result;
    xs_logd("model_count=%d", model->argc);
    for(i=0; i<model->argc; ++i)
    {
        xs_logd("model_%d=%s", i, model->argv[i]);
    }
    //xs_model_delete(model);
}

void myaccept(xs_ev_sock_t* sock)
{
    int fd;
    while((fd=accept(sock->fd, NULL, NULL)) > 0)
    {
        //xs_model_recv(fd, model_recv_cbk, NULL);
        //xs_model_recv(fd, model_recv_cb);
    }

    if(errno == EAGAIN)
        xs_ev_add(sock->ev);
    else
        xs_sock_close(sock);
}

int main(int argc, char** argv)
{
    argc = argc; argv = argv;
#if 0
    xs_server_init(4, NULL, argc, argv);

    xs_server_start_tcp(29999, 10, myaccept, NULL);

    xs_server_run();
#endif
    return 0;
}
