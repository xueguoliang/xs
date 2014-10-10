#ifndef CADDFRIEND_H
#define CADDFRIEND_H

#include <QDialog>
#include "main.h"

class CAddFriend : public QDialog
{
    Q_OBJECT
public:
    explicit CAddFriend(QString strUserName, QWidget *parent = 0);
    QLineEdit* _FriendName;
    QString _strUserName;
signals:
    void sigAddFriendSuccess(QString friendName);

public slots:
    void slotAddFriend();
};

#endif // CADDFRIEND_H
