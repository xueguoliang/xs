
#include "xs.h"

void handler_test(xs_model_t* model, int fd, struct xs_ctrl_t* ctrl)
{
    fd = fd;
    ctrl = ctrl;
    int i;
    for(i=0; i<model->argc; ++i)
    {
        xs_logd("argv[%d]=%s", i, model->argv[i]);
    } 
}

int main(int argc, char** argv)
{
    xs_server_init(4, NULL, argc, argv);
    xs_ctrl_t* ctrl = xs_ctrl_create(8999, NULL, NULL);
    xs_ctrl_reg_handler(ctrl, "test", "test", handler_test);
    return xs_server_run();
}
