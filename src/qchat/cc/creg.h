#ifndef CREG_H
#define CREG_H

#include "main.h"

class CReg : public QDialog
{
    Q_OBJECT
public:
    explicit CReg(QWidget *parent = 0);

    QLineEdit* _username;
    QLineEdit* _password;
    QLineEdit* _passwordAgain;

signals:

public slots:
    void doReg();
};

#endif // CREG_H
