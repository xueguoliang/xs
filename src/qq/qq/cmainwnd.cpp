#include "cmainwnd.h"
#include "clogin.h"
#include "creg.h"
#include "caddfriend.h"
#include "cchatwnd.h"

CMainWnd::CMainWnd(QWidget *parent) :
    QWidget(parent)
{
    QPushButton* Login;
    QPushButton* Reg;

    // 两个按钮的Layout
    QVBoxLayout* center = new QVBoxLayout;
    center->addWidget(Login = new QPushButton("Login"));
    center->addWidget(Reg = new QPushButton("Register"));

    // 把两个layout压缩到中间
    QGridLayout* layout = new QGridLayout(this);
    layout->addLayout(center, 1, 1);
    layout->setRowStretch(0, 1);
    layout->setRowStretch(2, 1);
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(2, 1);

    // 设置窗口大小
    resize(QSize(200, 500));
    connect(Login, SIGNAL(clicked()), this, SLOT(login()));
    connect(Reg, SIGNAL(clicked()), this, SLOT(reg()));

    // 把按钮对象保存到成员变量
    _login = Login;
    _reg = Reg;
}

void CMainWnd::login()
{
    CLogin login;
    if(login.exec() == QDialog::Accepted)
    {
        // 成功登录
        handleLoginSuccess(login._username->text());
    }
}

void CMainWnd::reg()
{
    CReg reg;
    if(reg.exec() == QDialog::Accepted)
    {
        // 成功注册
        handleLoginSuccess(reg._username->text());
    }
}

void CMainWnd::handleLoginSuccess(QString username)
{
    delete _login;
    delete  _reg;
    delete layout();
    // 保存登录或者注册成功的用户名
    _strUsername = username;

    QPushButton* addFrind;

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(new QLabel(username));
    mainLayout->addWidget(_listWidget = new QListWidget);
    mainLayout->addWidget(addFrind = new QPushButton("add friend"));

    connect(_listWidget, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(slotDoubleClickFriend(QModelIndex)));
    connect(addFrind, SIGNAL(clicked()), this, SLOT(slotAddFriend()));

    // 获取所有的好友信息
    int fd = xs_sock_connect(__xs_qq_port, __xs_qq_host);
    xs_model_send_block_arg(fd, 3, __xsc_userman,
                            __xsc_friendlist,
                            _strUsername.toUtf8().data());

    xs_model_t* model;
    xs_model_recv_block(&model, fd, 60000);
    ::close(fd);
    for(int i=0; i<model->argc; i++)
    {
        this->_listWidget->addItem(model->argv(i));
    }
    xs_model_delete(model);

    // 启动心跳定时器
    _timer = new QTimer(this);
    _timer->setInterval(5000);
    _timer->start();
    connect(_timer, SIGNAL(timeout()), this, SLOT(slotHeartBeat()));
}

void CMainWnd::slotHeartBeat()
{
    xs_model_t* model = qq_rpc_call(3, __xsc_userman,
                                    __xsc_heartbeat,
                                    _strUsername.toUtf8().data());
    if(model && xs_success(model->argv(0)))
    {
        if(QString("1") == model->argv(1))
        {
            slotRecvServerMessage();
        }
    }
}

void CMainWnd::slotAddFriend()
{
    CAddFriend addFriend(_strUsername);
    connect(&addFriend, SIGNAL(sigAddFriendSuccess(QString)),
            this, SLOT(slotAddFriendOK(QString)));
    addFriend.exec();
}

void CMainWnd::slotAddFriendOK(QString friendName)
{
    _listWidget->addItem(friendName);
}

void CMainWnd::slotDoubleClickFriend(QModelIndex index)
{
    QVariant var = _listWidget->model()->data(index); 
    getChatWnd(var.toString());
}

void CMainWnd::slotRecvServerMessage()
{
    xs_model_t* model = qq_rpc_call(3, __xsc_chat, __xsc_recv_msg,
                                    _strUsername.toUtf8().data());
    if(model)
    {
        if(xs_success(model->argv(0)))
        {
            // model(1) sender , model(2) recver, model(3) content
            CChatWnd* wnd = getChatWnd(model->argv(1));

            wnd->_show->append(model->argv(3));

            slotRecvServerMessage();
            return;
        }
    }

}

CChatWnd* CMainWnd::getChatWnd(QString peerName)
{
    CChatWnd* wnd = _chatWnds[peerName];
    if(!wnd)
    {
        wnd = _chatWnds[peerName] = new CChatWnd(_strUsername, peerName);
        connect(wnd, SIGNAL(sigServerHasMessage()),
                this, SLOT(slotRecvServerMessage()));
        wnd->show();
        wnd->setWindowTitle(QString().sprintf("与%s聊天中", peerName.toUtf8().data()));
        connect(wnd, SIGNAL(destroyed()), this, SLOT(slotChatWndDestroy()));

        wnd->setAttribute(Qt::WA_DeleteOnClose);
    }
    return wnd;
}

void CMainWnd::slotChatWndDestroy()
{
    CChatWnd* wnd = (CChatWnd*)sender();
    _chatWnds.remove(wnd->_strPeerUser);
    qDebug() << "aaa***" <<wnd->_strPeerUser;
}
