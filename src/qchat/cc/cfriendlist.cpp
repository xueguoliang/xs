#include "cfriendlist.h"


CFriendList::CFriendList(QString username, QWidget *parent) :
    CDelegate(parent), _username(username)
{
    QPushButton* button ;
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(_username));
    layout->addWidget(_friendList = new QListWidget);
    layout->addWidget(button = new QPushButton("加好友"));

    connect(button, SIGNAL(clicked()), this, SLOT(slotAddFriend()));

    loadFriends();
}

void CFriendList::loadFriends()
{
    xs_model_t* model = rpc_call(4, __xsc_userman, __xsc_friend_list,
                                 _username.toUtf8().data());
    if(model == NULL)
    {
        QMessageBox::warning(this, "Error", "getFriendList error");
        return;
    }
    this->_friendList->clear();
    for(int i=0; i<model->argc;++i)
    {
        this->_friendList->addItem(model->argv(i));
    }
}

void CFriendList::slotAddFriend()
{
    CAddFriend* panel = new CAddFriend(this->_username);
    panel->show();
    connect(panel, SIGNAL(sigAddFriendOK(QString)),
            this, SLOT(slotAddFriendOK(QString)));
}

void CFriendList::slotAddFriendOK(QString strUserPeer)
{
    this->_friendList->addItem(strUserPeer);
}
