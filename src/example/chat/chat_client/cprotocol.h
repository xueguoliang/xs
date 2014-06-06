#ifndef CPROTOCOL_H
#define CPROTOCOL_H

/*
    Protocol with server
    1) notify, just notify server, do not need response
    2) request, get data from server
    3) upload, send data to server, with other data, not need response
*/
#include "xs.h"
#include <QtCore>
#include "cc_model.h"

#define _SERVER_DEFAULT "127.0.0.1"
class CProtocol
{
public:
    CProtocol();

    QString _server;
    short _port;
    void setServerId(short port, const QString& server);

    static CProtocol* getInstance();

    xs_model_t* request(int fd, xs_model_t* model, const void*data = NULL, int len = 0);
    xs_model_t* login(ccm_login* model, 
            short port = _PORT_DEFAULT, const QString& server = _SERVER_DEFAULT);
    xs_model_t* reg(ccm_reg* model, 
            short port = _PORT_DEFAULT, const QString& server = _SERVER_DEFAULT);
};

#endif // CPROTOCOL_H
