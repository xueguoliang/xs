#include "cmainwnd.h"
#include "cmainmenu.h"
#include "cfriendlist.h"

CMainWnd::CMainWnd(QWidget *parent) :
    QWidget(parent)
{
    _layout = new QVBoxLayout(this);
    _layout->addWidget(_delegate = new CMainMenu);
    _layout->setMargin(0);
    _layout->setSpacing(0);

    connect(_delegate, SIGNAL(sigLoginSuccess(QString)),
            this, SLOT(slotLoginSuccess(QString)));
    connect(_delegate, SIGNAL(sigRegSuccess(QString)),
            this, SLOT(slotRegSuccess(QString)));

    resize(220, 600);
}

void CMainWnd::slotLoginSuccess(QString username)
{
    _layout->removeWidget(_delegate);
    delete _delegate;
    _layout->addWidget(_delegate = new CFriendList(username));
}
void CMainWnd::slotRegSuccess(QString username)
{
    slotLoginSuccess(username);
}

void CMainWnd::slotChatClose(QString /*peerUsername*/)
{

}
