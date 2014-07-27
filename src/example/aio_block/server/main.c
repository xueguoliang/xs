
#include "xs.h"

void login(xs_model_t* model, int fd, struct xs_ctrl_t* ctrl)
{
    model = model;
    xs_logd("login is called");
    ctrl->r(fd, ctrl);
}
void logout(xs_model_t* model, int fd, struct xs_ctrl_t* ctrl)
{
    model = model;
    xs_logd("logout is called");
    ctrl->r(fd, ctrl);
}
void reg(xs_model_t* model, int fd, struct xs_ctrl_t* ctrl)
{
    model = model;
    xs_logd("reg is called");
    ctrl->r(fd, ctrl);
}

int main(int argc, char** argv)
{
    xs_server_init(2, NULL, argc, argv);

    xs_ctrl_t* ctrl = xs_ctrl_create(9988, NULL, NULL);
    xs_ctrl_reg_handler(ctrl, "login", "login", login);
    xs_ctrl_reg_handler(ctrl, "login", "logout", logout);
    xs_ctrl_reg_handler(ctrl, "login", "reg", reg); 

    return xs_server_run();
}
