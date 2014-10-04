#include "clock.h"

CMutex::CMutex()
{
    pthread_mutex_init(&_mutex, NULL);
}
CMutex::~CMutex()
{
    pthread_mutex_destroy(&_mutex);
}

CLock::CLock(CMutex& mutex):_mutex(mutex)
{
    pthread_mutex_lock(&mutex._mutex);
}
CLock::~CLock()
{
    pthread_mutex_unlock(&_mutex._mutex);
}
