
#include "xs.h"
    
int main(int argc, char** argv)
{
    xs_init();

    char* p = xs_malloc(100);

    xs_fini();
    return 0;
}
