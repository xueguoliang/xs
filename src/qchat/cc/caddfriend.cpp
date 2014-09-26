#include "caddfriend.h"

CAddFriend::CAddFriend(QString username, QWidget *parent) :
    CDelegate(parent), _username(username)
{
    QVBoxLayout* vBox = new QVBoxLayout;
    QHBoxLayout* hBox1 = new QHBoxLayout;
    QHBoxLayout* hBox2 = new QHBoxLayout;
    vBox->addLayout(hBox1);
    vBox->addLayout(hBox2);
    setCenterLayout(vBox);

    hBox1->addWidget(new QLabel("好友编号："));
    hBox1->addWidget(this->_peerUser = new QLineEdit);
    hBox2->addStretch(1);
    QPushButton* btOK;
    QPushButton* btClose;
    hBox2->addWidget(btOK = new QPushButton("添加好友"));
    hBox2->addWidget(btClose = new QPushButton("关闭"));

    connect(btClose, SIGNAL(clicked()), this, SLOT(close()));
    connect(btOK, SIGNAL(clicked()), this, SLOT(slotAddFriend()));
}

void CAddFriend::slotAddFriend()
{
    /* send command to server, request add friend */
    if(this->_peerUser->text().length() == 0)
        return;
    int fd = xs_sock_connect(__XS_CHAT_PORT, "127.0.0.1");
    if(fd < 0)
    {
        QMessageBox::warning(this, "Error", "connect to server error");
        return;
    }

    int ret = xs_model_send_block_arg(fd, 4, __xsc_userman, __xsc_add_friend,
                      _username.toUtf8().data(), _peerUser->text().toUtf8().data());
    if(ret != 0)
    {
        QMessageBox::warning(this, "Error", "send add friend request failure");
        ::close(fd);
        return;
    }

    xs_model_t* model;
    ret = xs_model_recv_block(&model, fd, 60000);
    if(ret != 0 )
    {
        QMessageBox::warning(this, "Error", "recv data error");
        ::close(fd);
        return;
    }
    if( !xs_success(model->argv(0)))
    {
        QMessageBox::warning(this, "Error", QString("add friend failure:")+model->argv(1));
        ::close(fd);
        xs_model_delete(model);
        return ;
    }

    xs_model_delete(model);
    ::close(fd);
    emit sigAddFriendOK(_peerUser->text());
    deleteLater();
}
