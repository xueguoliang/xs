
#include "xs.h"

int main()
{
    xs_init();

    int fd = xs_sock_connect(8999, "127.0.0.1");
    xs_model_send_block_arg(fd, 4, "test", "test", "aaa", "bbb");

    close(fd);
    return 0;
}
