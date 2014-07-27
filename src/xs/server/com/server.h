
#ifndef __SERVER_H__
#define __SERVER_H__

#include "xs.h"
#include "def.h"


    #define CLI_SERVER_OBJ_SERVER "cli"
        #define CLI_SERVER_OP_INIT "init"

// reg cli command
extern void cli_reg_command(const char* cmd, xs_object_handler_t op);

#endif
