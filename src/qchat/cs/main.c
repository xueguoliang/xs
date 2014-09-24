
#include "main.h"



void qchat_reg(xs_model_t* model, int fd, struct xs_ctrl_t* ctrl)
{
    model = model;
    fd = fd;
    ctrl = ctrl;
    xs_logd("qchat_reg is called");

    model = xs_model_create(1);
    model->argv(0) = xs_strdup(__xs_ok);
    xs_model_send_and_close(fd, model);
}

int main(int argc, char* argv[])
{
    mysql_init(NULL);
    xs_server_init(4, NULL, argc, argv);

    xs_ctrl_t* ctrl = xs_ctrl_create(__xs_port, NULL, NULL);
    xs_ctrl_reg_handler(ctrl, __xsc_userman, __xsc_reg, qchat_reg);

    return xs_server_run();
}
