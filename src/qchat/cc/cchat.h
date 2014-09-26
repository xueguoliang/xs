#ifndef CCHAT_H
#define CCHAT_H

#include "main.h"

class CChat : public QWidget
{
    Q_OBJECT
public:
    explicit CChat(QWidget *parent = 0);
    QTextEdit* _input;
    QTextBrowser* _show;

signals:

public slots:

};

#endif // CCHAT_H
