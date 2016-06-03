
#include "xs_client.h"

void test()
{
    int fd = xs_sock_connect(12325, "127.0.0.1");
    if(fd < 0)
    {
        xs_dbg("connect server error\n");
    }

    xs_sock_send_block(fd, "abc", 4, 1000);

    char buf[1024];
    recv(fd, buf, sizeof(buf), 0);
}

int main(int argc, char* argv[])
{
    int i;
    int count = 20000;
    if(argc > 1)
    {
        count = atoi(argv[1]);
    }
    if(count <=0)
        count = 1;

    for(i=0;i <count; ++i)
    {
        pid_t pid = fork();
        if(pid ==0)
        {
            test();
            return 0;
        }
    }

    for(i=0; i<count; ++i)
    {
        wait(NULL);
    }

    return 0;
}

