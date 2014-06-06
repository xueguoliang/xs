#ifndef CC_MODEL_H
#define CC_MODEL_H
#include "xs.h"

#define _PORT_DEFAULT 9899

/* commands */
#define __cccmd_login "login"
#define __cccmd_reg "cmd"

typedef struct ccm_response
{
    int argc;
    char* result;
    char* reason;
}
ccm_response,
ccm_login_r;

typedef struct ccm_login
{
    int argc;
    char* cmd;
    char* username;
    char* password;
} ccm_login;

typedef struct ccm_reg
{
    int argc;
    char* cmd;
    char* username;
    char* password;
} ccm_register;

#define ccm_create(_Type, _varCount, ...) (_Type*)xs_model_create(_varCount, __VA_ARGS__)
#define ccm_isLogin(model)  strcmp(model->argv[0], __cccmd_login) == 0
#define ccm_isReg(model) strcmp(model->argv[0], __cccmd_reg) == 0

#endif // CC_MODEL_H
