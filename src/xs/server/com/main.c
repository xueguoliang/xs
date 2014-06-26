
#include "server.h"

int main(int argc, char** argv)
{
    xs_server_init(4, NULL, argc, argv);
    xs_ctrl_t* ctrl = xs_ctrl_create(CLI_SERVER_PORT, NULL, NULL);
    return xs_server_run();
}
