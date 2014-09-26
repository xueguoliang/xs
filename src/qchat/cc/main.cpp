
#include "main.h"
#include "cmainwnd.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    xs_init();

    CMainWnd mainWnd;
    mainWnd.show();

    app.exec();
    xs_fini();
    return 0;
}
