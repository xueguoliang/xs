

#include "xs.h"

void model_send_cb(xs_model_t* model, void* ptr, int result)
{
    ptr = ptr;
    result = result;
    model = model;
}

int main(int argc, char** argv)
{
    argc = argc; argv = argv;
    xs_server_init(4, NULL, argc, argv);

    int fd = xs_sock_connect(29999, "127.0.0.1");
    xs_model_send_arg(fd, 3, "a", "b", "c");

//    xs_model_t* model = xs_model_create(3, "a", "b", "c");
//    xs_model_send(fd, model, model_send_cb, NULL);

    xs_server_run();
    return 0;
}
