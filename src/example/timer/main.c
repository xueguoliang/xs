
#include "xs.h"

void timer_callback(xs_ev_time_t* tev)
{
    tev = tev;
    xs_logd("timeout");
}

int main(int argc, char** argv)
{
    xs_server_init(4, NULL, argc, argv);                /* start server with 4 thread */
    xs_ev_add_time_ev(1000, timer_callback, NULL);      /* set timer 1 second */
    xs_server_run();                                    /* server run */

    return 0;
}
