#include "caddfriend.h"

CAddFriend::CAddFriend(QString strUserName, QWidget *parent) :
    QDialog(parent), _strUserName(strUserName)
{
    QPushButton* button;
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(this->_FriendName = new QLineEdit);
    layout->addWidget(button = new QPushButton("AddFriend"));
    connect(button, SIGNAL(clicked()), this, SLOT(slotAddFriend()));
}

void CAddFriend::slotAddFriend()
{
    if(_FriendName->text().length() == 0)
        return;
#if 0
    int fd = xs_sock_connect(__xs_qq_port, __xs_qq_host);
    xs_model_send_block_arg(fd, 4, __xsc_userman, __xsc_add_friend,
                           _strUserName.toUtf8().data(),
                            _FriendName->text().toUtf8().data());
    xs_model_t* model;
    xs_model_recv_block(&model, fd, 60000);
    ::close(fd);
#endif
    xs_model_t* model = qq_rpc_call(4, __xsc_userman, __xsc_add_friend,
                                    _strUserName.toUtf8().data(),
                                     _FriendName->text().toUtf8().data());
    if(model == NULL)
    {
        QMessageBox::warning(this, "Error", "Model is NULL");
        return;
    }
    if(xs_success(model->argv(0)))
    {
        emit sigAddFriendSuccess(_FriendName->text());
    }
    else
    {
        QMessageBox::warning(this, "Error", model->argv(1));
    }
    xs_model_delete(model);
}
