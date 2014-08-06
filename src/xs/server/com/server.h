
#ifndef __SERVER_H__
#define __SERVER_H__

#include "xs.h"
#include "def.h"

// reg cli command
extern void cli_reg_command(const char* cmd, xs_object_handler_t op);

#define IS_PORT 9911
#define IS_IP "127.0.0.1"

static inline void rsp_error(const char* reason, int fd)
{
    xs_model_t* rsp = xs_model_create_v(2, __xs_err, reason);
    xs_model_send_and_close(fd, rsp);
    xs_model_delete(rsp);
    return;
}

#endif
