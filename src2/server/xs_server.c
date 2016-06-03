
#include "xs_server.h"
#include "xs_recv.h"
#include "xs_db.h"
typedef struct xs_process_t
{
    pid_t pid;
} xs_process_t;

xs_vec_t* processes = NULL;

void xs_process_add(pid_t pid)
{
    xs_process_t* process = xs_malloc(sizeof(*process));
    process->pid = pid;

    xs_vec_add(processes, process);
}

int xs_fork_main(int server_fd)
{
    int epoll = epoll_create(1024);
    xs_epoll_add(epoll, server_fd, EPOLLIN, xs_get_ctrl(server_fd));

    xs_db_init();

    int count = 8;
    struct epoll_event* evs = xs_malloc(sizeof(*evs)*count);

    xs_dbg("worker loop\n");

    while(1)
    {
        int ret = epoll_wait(epoll, evs, count, 10000);
        if(ret > 0)
        {
            int i;
            for(i=0; i<ret; ++i)
            {
                xs_recv_ctrl_t* ctrl = evs[i].data.ptr;

                if(ctrl->fd == server_fd)
                {
                    while(1)
                    {
                        XSOCKET fd = accept(server_fd, NULL, NULL);
                        if(fd < 0)
                        {
                            break;
                        }

                        xs_dbg("new connect fd=%d\n", fd);

                        xs_epoll_add(epoll, fd, EPOLLIN|EPOLLET, xs_get_ctrl(fd));
                        xs_sock_set_nonblock(fd);
                    }


                }
                else
                {

                    xs_server_recv(ctrl);
                }
            }

            if(ret == count)
            {
                count *= 2;
                evs = xs_realloc(evs, sizeof(*evs)*count);
            }
        }
        else
        {
            if(ret < 0 && errno != EINTR)
            {
                break;
            }
        }
    }

    xs_db_fini();
    
    return 0;
}

int main()
{
    int fd = xs_sock_server(xs_port_server, "0.0.0.0", 200);

    int manager_fd = xs_sock_server(xs_port_manager, "127.0.0.1", 5);

    int manager_epoll = epoll_create(512);
    xs_epoll_add(manager_epoll, manager_fd, EPOLLIN, NULL);

    processes = xs_vec_create(20);

    xs_dbg("enter loop\n");

    while(1)
    {
        struct epoll_event ev;
        int ret = epoll_wait(manager_epoll, &ev, 1, 60*1000);
        if(ret > 0)
        {
            if(ev.data.fd == manager_fd)
            {
                int fd = accept(manager_fd, NULL, NULL);
                // just add one
                xs_epoll_add(manager_epoll, fd, EPOLLIN, NULL);
            }
            else
            {
                char buf;
                int ret = recv(ev.data.fd, &buf, 1, 0);
                if(ret > 0)
                {
                    if(buf == 'A')
                    {
                        pid_t pid = fork();
                        if(pid == 0)
                        {
                            close(manager_fd);
                            close(manager_epoll);
                            xs_vec_destroy(processes, free);
                            xs_dbg("create new worker\n");
                            return xs_fork_main(fd);
                        }
                        else
                        {
                            xs_process_add(pid);
                            xs_dbg("process count=%d\n", processes->count);
                        }

                    }
                    else if(buf == 'D')
                    {
                        // TODO send message to child to quit
                    }
                    else
                    {
                        printf("what is this: %c\n", buf);
                    }
                }
                else // ret <= 0, close socket
                {
                    xs_close_socket(ev.data.fd);
                }
            }
        }
    }

    return 0;
}

