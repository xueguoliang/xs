
#include "main.h"
#include "CReg.h"
#include "CLogin.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    xs_init();

    CLogin reg;
    reg.exec();

    return app.exec();
}
