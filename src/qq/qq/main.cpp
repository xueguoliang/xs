
#include "xs.h"
#include <QApplication>
#include "cmainwnd.h"

xs_model_t* qq_rpc_call(int argc, ...)
{
    int fd = xs_sock_connect(__xs_qq_port, __xs_qq_host);
    if(fd < 0)
    {
        xs_logd("connect server error");
        return NULL;
    }

    va_list ap;
    va_start(ap, argc);
    xs_model_t* model = xs_model_create_ap(argc, ap);
    va_end(ap);
    // 往服务器发送model
    int ret = xs_model_send_block(model, fd, 60000);
    if(ret < 0)
    {
        xs_logd("send data to server error");
        return NULL;
    }

    // 从服务器获取相应
    ret = xs_model_recv_block(&model, fd, 60000);
    if(ret < 0)
    {
        xs_logd("recv data from server error");
        return NULL;
    }
    close(fd);
    return model;
}

int main(int argc, char** argv)
{
    xs_init();
    QApplication app(argc, argv);

    CMainWnd wnd;
    wnd.show();

    int ret = app.exec();
    xs_fini(); // 程序结束时检查内存泄漏
    return ret;
}
