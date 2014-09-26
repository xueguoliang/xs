
#include "xs.h"

int main()
{
    xs_init();

    int fd = xs_sock_connect(9988, "127.0.0.1");

    xs_model_send_block_arg(fd, 4, "login", "login", "aaa", "bbb");
    xs_model_send_block_arg(fd, 2, "login", "logout");

    getchar();

    sleep(10000);

    close(fd);
    return 0;
}
