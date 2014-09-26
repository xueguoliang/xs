#ifndef CREQUEST_H
#define CREQUEST_H
#include "main.h"
class CRequest
{
public:
    CRequest();

    xs_model_t* rpc_call(int argc, ...);
};

#endif // CREQUEST_H
