
#include "xs.h"

int main()
{
    xs_init();
    void* p = xs_malloc(100);

    xs_fini();
    return 0;
}
