
#include "xs.h"
    
char buf[] = "hello, this is aio send";
char buftosend[1024];
int fd;

void aio_send_done(xs_aio_t* aio)
{
    if(aio->result == XS_AIO_DONE)
    {
        sleep(2);
        xs_aio_send_no_header(fd, aio_send_done, xs_strdup(buf), sizeof(buf), NULL);
    }
    else
    {
        close(aio->fd);
    }
    xs_aio_free(aio);
}

int main(int argc, char** argv)
{
    xs_server_init(4, NULL, argc, argv);

    fd = xs_sock_connect(19999, "127.0.0.1");

    xs_aio_send_no_header(fd, aio_send_done, xs_strdup(buf), sizeof(buf), NULL);

    xs_server_run();
    return 0;
}
