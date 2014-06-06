
#include "xs.h"

void myrecv(xs_ev_sock_t* sock)
{
    int ret;
    char ch;
    while(1)
    {
        ret = recv(sock->fd, &ch, 1, 0);
        if(ret > 0)
            printf("%c", ch);
        else
            break;
    }

    if(errno == EAGAIN)
    {
        xs_ev_add(sock->ev);
    }
    else
    {
        xs_sock_close(sock);
    }
}

void myaccept(xs_ev_sock_t* sock)
{
    while(1)
    {
        int fd = accept(sock->fd, NULL, NULL);
        if(fd < 0)
            break;

        xs_ev_add_sock_ev(fd,  EPOLLIN, myrecv, NULL);
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
