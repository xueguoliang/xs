#ifndef CMAINWND_H
#define CMAINWND_H

#include "main.h"
#include "cdelegate.h"
#include "cchat.h"

class CMainWnd : public QWidget
{
    Q_OBJECT
public:
    explicit CMainWnd(QWidget *parent = 0);
    CDelegate* _delegate;
    QVBoxLayout* _layout;
    QMap<QString, CChat*> _chats;
signals:

public slots:
    void slotLoginSuccess(QString username);
    void slotRegSuccess(QString username);
    void slotChatClose(QString peerUsername);
};

#endif // CMAINWND_H
