#ifndef CLOGIN_H
#define CLOGIN_H

#include "main.h"

class CLogin : public QDialog
{
    Q_OBJECT
public:
    explicit CLogin(QWidget *parent = 0);

    QLineEdit* _username;
    QLineEdit* _password;

signals:

public slots:
    void doLogin();

};

#endif // CLOGIN_H
