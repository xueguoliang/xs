
#include "xs_manager.h"

int main()
{
    int fd = xs_sock_connect(12326, "127.0.0.1");
    if(fd < 0)
    {
        xs_dbg("connect server error\n");
    }
    send(fd, "A", 1, 0);

    return 0;
}

