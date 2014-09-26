#ifndef CFRIENDLIST_H
#define CFRIENDLIST_H

#include "main.h"
#include "cdelegate.h"
#include "caddfriend.h"
#include "crequest.h"

class CFriendList : public CDelegate, public CRequest
{
    Q_OBJECT
public:
    explicit CFriendList(QString username, QWidget *parent = 0);
    QListWidget* _friendList;
    QString _username;

    void loadFriends();
signals:

public slots:
    void slotAddFriend();
    void slotAddFriendOK(QString strUserPeer);
};

#endif // CFRIENDLIST_H
