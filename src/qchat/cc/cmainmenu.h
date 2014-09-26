#ifndef CMAINMENU_H
#define CMAINMENU_H

#include "main.h"
#include "cdelegate.h"

class CMainMenu : public CDelegate
{
    Q_OBJECT
public:
    explicit CMainMenu(QWidget *parent = 0);

signals:

public slots:
    void login();
    void reg();

};

#endif // CMAINMENU_H
