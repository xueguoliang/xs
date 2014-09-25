/*
 * This file is part of the xs Library.
 *
 * Copyright (C) 2011 by Guoliang Xue <xueguoliang@gmail.com>
 *
 * The xs Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The xs Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the xs Library. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __SERVER_H__
#define __SERVER_H__

#include "xs.h"
#include "def.h"

// reg cli command
extern void cli_reg_command(const char* cmd, xs_object_handler_t op);

// bs to is
#define __xs_bs2is "bs2is"
#define __xs_reg "reg"
typedef struct model_bs2is_reg_t
{
    intptr_t argc;
    char* type;             // for command
    char* command;

    char* name;
    char* address;
    char* cpu;
    char* memory;
    char* ip;
    char* port;
    char* max_user;
} model_bs2is_reg_t;

            
#define __xs_cs "cs"
#define __xs_req_service "req_service"

typedef struct model_cs2is_req_service_t
{
    intptr_t argc;
    char* command_type;
    char* command;
    char* req_type;
    char* session_name;
} model_cs2is_req_service_t;

typedef struct model_cs2is_req_service_resp_t
{
    intptr_t argc;
    char* session_name;
    char* ipaddr;
    char* port;
} model_cs2is_req_service_resp_t;

#define IS_PORT 9911
#define IS_IP "127.0.0.1"

static inline void xs_rsp_error(const char* reason, int fd)
{
    xs_model_t* rsp = xs_model_create_v(2, __xs_err, reason);
    xs_model_send_and_close(fd, rsp);
    return;
}

static inline void xs_rsp_ok(int fd)
{
    xs_model_t* rsp = xs_model_create_v(1, __xs_ok);
    xs_model_send_and_close(fd, rsp);
    return;
}

static inline void xs_rsp(int fd, int argc, ...)
{
    va_list ap;
    va_start(ap, argc);
    xs_model_t* model = xs_model_create_ap(argc, ap);
    va_end(ap);
    xs_model_send_and_close(fd, model);
    return;
}

#endif
