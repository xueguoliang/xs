
#include "main.h"
#include "creg.h"
#include "clogin.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    xs_init();


    CLogin reg;
    reg.exec();

  //  app.exec();
    xs_fini();
    return 0;
}
