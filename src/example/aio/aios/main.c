
#include "xs.h"



void aio_done(xs_aio_t* aio)
{
    if(aio->result == XS_AIO_DONE)
    {
        xs_logd("******************aio recv buf=%s, len=%d", aio->buf, aio->buflen);
        // 处理

        xs_aio_recv(aio->fd, aio_done, NULL);  /* read more data */
    }
    else
    {
        close(aio->fd);
        xs_logd("*******************************failure aio recv");
    }

    xs_aio_free(aio);
}

void myaccept(xs_ev_sock_t* sock)
{
    int fd;
    while((fd=accept(sock->fd, NULL, NULL)) > 0)
    {
        // [报文长度][报文内容]
        xs_aio_recv(fd, aio_done, NULL);
    }

    if(errno == EAGAIN)
        xs_ev_add(sock->ev);
    else
        xs_sock_close(sock);
}

int main(int argc, char** argv)
{
    xs_server_init(4, NULL, argc, argv);

    xs_server_start_tcp(19999, 10, myaccept, NULL);

    xs_server_run();

    return 0;
}
