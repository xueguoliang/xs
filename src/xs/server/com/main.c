
#include "server.h"

extern void server_init();

typedef struct server_clis_t
{
    xs_ctrl_t*      ctrl;
    xs_vec_t*       cmds;
} server_clis_t;

server_clis_t g_clis;

// typedef void (*xs_object_handler_t)(xs_model_t* model, int fd, struct xs_ctrl_t* ctrl);
void cli_reg_command(const char* cmd, xs_object_handler_t op)
{
    char* p = xs_strdup(cmd);

    char* saveptr;
    char* p1 = strtok_r(p, " ", &saveptr);
    char* p2 = strtok_r(NULL, " ", &saveptr);
    xs_ctrl_reg_handler(g_clis.ctrl, xs_strdup(p1), xs_strdup(p2), op);
    xs_vec_add(g_clis.cmds, xs_strdup(cmd));

    xs_free(p);
}

 void cli_init(xs_model_t* model, int fd, struct xs_ctrl_t* ctrl)
 {
     assert(model->argc == 2);
     ctrl = ctrl;

     xs_logd("cli_init start");

     xs_model_t* rsp = xs_model_create(g_clis.cmds->count);
     int i;
     for(i=0; i<g_clis.cmds->count; ++i)
     {
        rsp->argv[i] = xs_strdup((const char*)xs_vec_get(g_clis.cmds, i));
     }
     xs_model_send_and_close(fd, rsp);
     xs_model_delete(rsp);

     xs_logd("cli_init leave");
 }
 void cli_show(xs_model_t* model, int fd, struct xs_ctrl_t* ctrl)
 {
     cli_init(model, fd, ctrl);
 }
 void cli_mem_check(xs_model_t* model, int fd, struct xs_ctrl_t* ctrl)
 {
     model = model;
     fd = fd;
     ctrl = ctrl;

     xs_ev_add_msg_ev(XS_EV_MSG_MEM_CHECK, 0, NULL, NULL);

     xs_model_t* rsp = xs_model_create_v(1, __xs_ok);
     xs_model_send_and_close(fd, rsp);
     xs_model_delete(rsp);
 }

int main(int argc, char** argv)
{
    xs_server_init(4, NULL, argc, argv);
    
    g_clis.ctrl = xs_ctrl_create(CLI_SERVER_PORT, NULL, NULL);
    g_clis.cmds = xs_vec_create(4);

    cli_reg_command(__xs_clis " " __xs_init, cli_init);

    cli_reg_command(__xs_clis " " __xs_show, cli_show);
    cli_reg_command(__xs_memory " " __xs_check, cli_mem_check);
    server_init();

    return xs_server_run();
}
