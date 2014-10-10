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
    QLineEdit* _password2;

signals:

public slots:
    void reg();
};

#endif // CREG_H
