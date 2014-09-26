#include "cmainmenu.h"
#include "clogin.h"
#include "creg.h"

CMainMenu::CMainMenu(QWidget *parent) :
    CDelegate(parent)
{
    QPushButton* Login = new QPushButton("登录");
    QPushButton* Reg = new QPushButton("没有帐号");

    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(Login, 1, 1);
    layout->addWidget(Reg, 2, 1);
    layout->setRowStretch(0, 1);
    layout->setRowStretch(3, 1);
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(2, 1);

    connect(Login, SIGNAL(clicked()), this, SLOT(login()));
    connect(Reg, SIGNAL(clicked()), this, SLOT(reg()));
}

void CMainMenu::login()
{
    CLogin login;
    if(login.exec() == QDialog::Accepted)
    {
        emit sigLoginSuccess(login._username->text());
    }
}

void CMainMenu::reg()
{
    CReg reg;
    if(reg.exec() == QDialog::Accepted)
    {
        emit sigRegSuccess(reg._username->text());
    }

}
