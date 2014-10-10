#include "cchatwnd.h"

CChatWnd::CChatWnd(QString strUsername, QString strPeerUser, QWidget *parent) :
    QWidget(parent), _strUsername(strUsername), _strPeerUser(strPeerUser)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(this->_show = new QTextBrowser, 3);
    layout->addWidget(this->_input = new QTextEdit, 1);
    layout->addWidget(this->_send = new QPushButton("Send"));

    connect(_send, SIGNAL(clicked()), this, SLOT(slotSend()));
}

void CChatWnd::slotSend()
{
    QString str = _input->toPlainText();
    if(str.length() == 0)
        return;
    xs_model_t* model = qq_rpc_call(5,
                                    __xsc_chat,
                                    __xsc_chat,
                                    _strUsername.toUtf8().data(),
                                    _strPeerUser.toUtf8().data(),
                                    str.toUtf8().data());
    if(model == NULL)
    {
        QMessageBox::warning(this, "Error", "Send error");
        return;
    }
    if(xs_success(model->argv(0)))
    {
        if(QString(model->argv(1)) == "1")
        {
            // 服务器有我的消息需要接收
           emit sigServerHasMessage();
        }
    }
    xs_model_delete(model);

    _show->append(str);
    _input->clear();
}


