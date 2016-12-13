
#include "xs.h"

void timer_callback(xs_ev_time_t* tev)
{
    tev = tev;
    xs_logd("timeout");
}

int main(int argc, char** argv)
{
#if 0
    xs_server_init(4, NULL, argc, argv);                /* start server with 4 thread */
    xs_ev_add_time_ev(1000, timer_callback, NULL);      /* set timer 1 second */
    xs_server_run();                                    /* server run */
#endif

    xs_init();

    int i;
    int count = 10000*10000;

    for(i=0;i <count; ++i)
    {
        void* p = xs_malloc(100);
        xs_free(p);
    }
    xs_fini();
    return 0;
}
