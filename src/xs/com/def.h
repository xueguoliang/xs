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

#ifndef __XS_COM_DEF__
#define __XS_COM_DEF__

// command
#define __xs_clis "clis"
#define __xs_init "init"
#define __xs_show "show"
#define __xs_memory "mem"
#define __xs_check "check"

// user to server
#define __xs_user "user"
#define __xs_req_service "req_service"
// request one server
typedef struct model_user_req_service
{
    intptr_t argc;
    char* cmd1;
    char* cmd2;

    char* service_type;
} model_user_req_service;

// common command


#define CLI_SERVER_PORT 9899
#define CS_PORT 9999


#endif
