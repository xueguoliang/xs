#include "mainwnd.h"
#include <QApplication>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include "cc_model.h"
#include "cprotocol.h"

Login::Login(QWidget *parent) :
    QWidget(parent)
{
    _username = new QLineEdit();
    _password = new QLineEdit();

    QPushButton* btnLogin = new QPushButton("Login");
    QPushButton* btnReg = new QPushButton("Reg");
    QPushButton* btnQuit = new QPushButton("Bye");

    QLabel* lUsername = new QLabel("Username:");
    QLabel* lPassword = new QLabel("Password:");

    QVBoxLayout* layoutMain = new QVBoxLayout(this);
    QGridLayout* layoutUp = new QGridLayout();
    QHBoxLayout* layoutDown = new QHBoxLayout();

    layoutMain->addLayout(layoutUp);
    layoutMain->addLayout(layoutDown);

    layoutUp->addWidget(lUsername, 0, 0);
    layoutUp->addWidget(lPassword, 1, 0);
    layoutUp->addWidget(_username, 0, 1);
    layoutUp->addWidget(_password, 1, 1);

    layoutDown->addStretch();
    layoutDown->addWidget(btnLogin);
    layoutDown->addWidget(btnReg);
    layoutDown->addSpacing(20);
    layoutDown->addWidget(btnQuit);

    this->connect(btnLogin, SIGNAL(clicked()), this, SLOT(login()));
    this->connect(btnReg, SIGNAL(clicked()), this, SLOT(reg()));
    this->connect(btnQuit, SIGNAL(clicked()), this, SLOT(close()));
}

void Login::login()
{
    ccm_login* cc_login = ccm_create(ccm_login, 3,
            __cccmd_login,
                                     _username->text().toUtf8().data(),
                                     _password->text().toUtf8().data());

    ccm_response* ret = (ccm_response*)CProtocol::getInstance()->login(cc_login);
    if(!ret)
    {
        QMessageBox::warning(this, "Warning", "network error");
        xs_model_delete((xs_model_t*)cc_login);
        return;
    }

    if(xs_success(ret->result))
    {
        MainWnd* wnd = new MainWnd();
        wnd->show();
        close();
    }
    else
    {
        QMessageBox::warning(this, "Warning", ret->reason);
    }
    xs_model_delete((xs_model_t*)ret);
    xs_model_delete((xs_model_t*)cc_login);
}

void Login::reg()
{
    ccm_reg* cc_reg = ccm_create(ccm_reg, 3, 
            __cccmd_reg,
                    _username->text().toUtf8().data(),
                    _password->text().toUtf8().data());
    ccm_response* ret = (ccm_response*)CProtocol::getInstance()->reg(cc_reg);

    if(!ret)
    {
        QMessageBox::warning(this, "Warning", "network error");
        xs_model_delete((xs_model_t*)cc_reg);
        return;
    }

    if(xs_success(ret->result))
    {
        QMessageBox::warning(this, "Info", "reg success, login now");
        login();     
    }
    else
    {
        QMessageBox::warning(this, "Warning", ret->reason);
    }
    xs_model_delete((xs_model_t*)ret);
    xs_model_delete((xs_model_t*)cc_reg);
}

MainWnd::MainWnd(QWidget *parent) :
    QWidget(parent)
{
}

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    xs_init();
    Login wnd;
    wnd.show();
    app.exec();
}

