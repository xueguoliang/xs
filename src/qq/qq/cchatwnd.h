#ifndef CCHATWND_H
#define CCHATWND_H

#include <QWidget>
#include "main.h"

class CChatWnd : public QWidget
{
    Q_OBJECT
public:
    explicit CChatWnd(QString strUsername, QString strPeerUser, QWidget *parent = 0);
    QString _strUsername;
    QString _strPeerUser;

    QTextBrowser* _show;
    QTextEdit* _input;
    QPushButton* _send;
signals:
    void sigServerHasMessage();

public slots:
    void slotSend();
};

#endif // CCHATWND_H
