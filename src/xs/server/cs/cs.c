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

#include "server.h"

typedef struct cs_session_t
{
    int     resp_fd;
    char*   session_name;
} cs_session_t;

typedef struct cs_t
{
    xs_hash_t*  sessions;
    xs_ctrl_t*  ctrl;
} cs_t;

static cs_t g_cs;


void cs_del_session(cs_session_t* session)
{
    xs_free(session->session_name);
    xs_free(session);
}

void cs_del_session_cbk(xs_hash_node_t* node)
{
    cs_del_session(node->value);
    xs_free(node);
}

cs_session_t* cs_new_session(int fd)
{
    cs_session_t* session = xs_malloc(sizeof(*session));

    session->resp_fd = fd;
    session->session_name = xs_gen_session_name(fd);

    return session;
}
void cs_req_recv_cbk(xs_model_cb_t* cb)
{
    if(cb->aio->result == XS_AIO_DONE)
    {
        // recv OK
        model_cs2is_req_service_resp_t* m = 
            (model_cs2is_req_service_resp_t*)cb->model;
        cs_session_t* session = (cs_session_t*)cb->ptr;

        xs_rsp(session->resp_fd, 3, __xs_ok, m->ipaddr, m->port);
    }
    else
    {
                
    }
}

void cs_req_send_cbk(xs_model_cb_t* cb)
{
    
    if(cb->aio->result == XS_AIO_DONE) 
    {
        // send success
        xs_model_recv(cb->aio->fd, cs_req_recv_cbk, cb->ptr);
    }
    else
    {
        // send failure, remove the session
        cs_session_t* session = (cs_session_t*)cb->ptr;
        xs_rsp_error("IS error", session->resp_fd);
        xs_hash_del(g_cs.sessions, session->session_name, cs_del_session_cbk);
    }

    xs_model_delete(cb->model);
}

void cs_user_req_service(xs_model_t* model, int fd, struct xs_ctrl_t* ctrl)
{
    ctrl = ctrl;
    // connect to is, if failure, send error information to client
    int fd1 = xs_sock_connect(IS_PORT, IS_IP);
    if(fd1 < 0)
    {
        xs_rsp_error("IS down", fd);
        return;
    }

    model_user_req_service* m = (model_user_req_service*)model;
    
    // 产生一个session_name
    // 创建一个session结构并加入到hash
    // 创建一个model发送给is服务器
    // 接收is服务器的回应
    // 等到is回应之后返回给客户端
    cs_session_t* session = cs_new_session(fd);
    xs_hash_add(g_cs.sessions, session->session_name, session);

    model = xs_model_create_v(4, 
            __xs_cs, 
            __xs_req_service, 
            m->service_type, 
            session->session_name);
    

    xs_model_send(fd1, cs_req_send_cbk, session, model);

#if 0
    int ret = xs_model_send_block(model, fd1, 60000);
    if(ret != 0)
    {
        xs_rsp_error("send IS info failure", fd);
        close(fd1);
        return;
    }

    xs_model_t* rsp;
    ret = xs_model_recv_block(&rsp, fd1, 60000);
    if(ret != 0)
    {
        xs_rsp_error("recv IS info failure", fd);
        close(fd1);
        return;
    }

    // rsp the bs to client
    xs_model_send_and_close(fd, rsp);
#endif
}

void server_init()
{
    g_cs.sessions = xs_hash_create();
    g_cs.ctrl = xs_ctrl_create(CS_PORT, NULL, NULL);
    // client send the req service to server
    xs_ctrl_reg_handler(g_cs.ctrl, __xs_user, __xs_req_service, cs_user_req_service);
}




