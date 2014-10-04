#ifndef CLOCK_H
#define CLOCK_H
#include <pthread.h>

class CMutex
{
    friend class CLock;
public:
    CMutex();
    ~CMutex();
private:
    pthread_mutex_t _mutex;
};

class CLock
{
public:
    CLock(CMutex& mutex);
    ~CLock();
    CMutex& _mutex;
};

#endif // CLOCK_H
