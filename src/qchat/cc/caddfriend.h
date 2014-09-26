#ifndef CADDFRIEND_H
#define CADDFRIEND_H
#include "main.h"
#include "cdelegate.h"

class CAddFriend : public CDelegate
{
    Q_OBJECT
public:
    explicit CAddFriend(QString username, QWidget *parent = 0);
    QString _username;
    QLineEdit* _peerUser;
signals:

public slots:
    void slotAddFriend();
};

#endif // CADDFRIEND_H
