#ifndef CMAINWND_H
#define CMAINWND_H

#include "main.h"
#include "cchatwnd.h"

class CMainWnd : public QWidget
{
    Q_OBJECT
public:
    explicit CMainWnd(QWidget *parent = 0);

    void handleLoginSuccess(QString username);
    QPushButton* _login;
    QPushButton* _reg;
    QString _strUsername;
    QListWidget* _listWidget;
    QMap<QString, CChatWnd*> _chatWnds;
    CChatWnd* getChatWnd(QString peerName);
    QTimer* _timer;
signals:


public slots:
    void login();
    void reg();
    void slotAddFriend();
    void slotAddFriendOK(QString friendName);
    void slotDoubleClickFriend(QModelIndex index);
    void slotRecvServerMessage();
    void slotHeartBeat();
    void slotChatWndDestroy();
};

#endif // CMAINWND_H
