
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
        // 重新把socket加入到epoll
        xs_ev_add(sock->ev);
    }
    else
    {
        // 关闭socket，如果有错误，则关闭socket
        xs_sock_close(sock);
    }
}

void myaccept(xs_ev_sock_t* sock)
{
    // 一次性接收所有的连接
    while(1)
    {
        int fd = accept(sock->fd, NULL, NULL);
        if(fd < 0)
            break;

        // 放入epoll集合中 
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
