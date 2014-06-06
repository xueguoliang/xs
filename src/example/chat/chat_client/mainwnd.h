#ifndef MAINWND_H
#define MAINWND_H

#include <QWidget>
class QLineEdit;

class Login : public QWidget
{
    Q_OBJECT
public:
    explicit Login(QWidget* parent = 0);

    QLineEdit* _username;
    QLineEdit* _password;

signals:

public slots:
    void login();
    void reg();
};

class MainWnd : public QWidget
{
    Q_OBJECT
public:
    explicit MainWnd(QWidget *parent = 0);

signals:

public slots:

};

#endif // MAINWND_H
