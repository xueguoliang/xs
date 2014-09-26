#ifndef CDELEGATE_H
#define CDELEGATE_H

#include "main.h"

class CDelegate : public QWidget
{
    Q_OBJECT
public:
    explicit CDelegate(QWidget *parent = 0);
    void setCenterLayout(QLayout* layout)
    {
        QGridLayout* grid = new QGridLayout(this);
        grid->addLayout(layout, 1, 1);
        grid->setRowStretch(0, 1);
        grid->setRowStretch(2, 1);
        grid->setColumnStretch(0, 1);
        grid->setColumnStretch(2, 1);
    }

signals:
    void sigLoginSuccess(QString username);
    void sigRegSuccess(QString username);
    void sigAddFriendOK(QString peerUser);

public slots:

};

#endif // CDELEGATE_H
