
#include "xs.h"

int main()
{
    xs_init();
        
    char ch;
    int fd = xs_sock_connect(19999, "127.0.0.1");

    while(1)
    {
        ch = (char)getchar();
        send(fd, &ch, 1, 0);
    }

    xs_fini();
    return 0;
}
