#include "cprotocol.h"
static CProtocol* theOnlyProtocolObject = NULL;

CProtocol::CProtocol()
{
    theOnlyProtocolObject = this;
}

CProtocol* CProtocol::getInstance()
{
    if(theOnlyProtocolObject) return theOnlyProtocolObject;
    return new CProtocol;
}

void CProtocol::setServerId(short port, const QString &server)
{
    _port = port;
    _server = server;
}

xs_model_t* CProtocol::login(ccm_login *model, short port, const QString &server)
{
    int fd = xs_sock_connect(port, server.toUtf8().data());
    if(fd < 0)
    {
        xs_logd("connect to server error");
        return NULL;
    }

    xs_model_t* ret = request(fd, (xs_model_t*)model);
    close(fd);

    return ret;
}

xs_model_t* CProtocol::reg(ccm_reg* model, short port, const QString& server)
{
    int fd = xs_sock_connect(port, server.toUtf8().data());
    if(fd < 0)
    {
        xs_logd("connect to server error");
        return NULL;
    }

    xs_model_t* ret = request(fd, (xs_model_t*)model);
    close(fd);

    return ret;
}

xs_model_t* CProtocol::request(int fd, xs_model_t *model, const void *data, int len)
{
    int ret = xs_model_send_block(model, fd, 60000);
    if(ret < 0)
    {
        xs_logd("send cmd error");
        return NULL;
    }

    if(data)
    {
        ret = xs_sock_send_block(fd, (char*)data, len, 6000);
        if(ret < 0)
        {
            xs_logd("send data error");
            return NULL;
        }
    }

    xs_model_t* resp = NULL;
    if(xs_model_recv_block(&resp, fd, 60000) < 0)
    {
        xs_logd("recv response error");
        return NULL;
    }
    return resp;

}

