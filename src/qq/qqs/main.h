#ifndef MAIN_H
#define MAIN_H
#include <QSqlTableModel>
#include <QList>
#include <QVariant>
#include <QDebug>
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlRecord>
#include "xs.h"
#include "../qqdef.h"

class CMutex
{
public:
    pthread_mutex_t _mutex;
    CMutex()
    {
        pthread_mutex_init(&_mutex, NULL);
    }
    ~CMutex()
    {
        pthread_mutex_destroy(&_mutex);
    }
};

class CLock
{
public:
    CMutex& _mutex;
    CLock(CMutex& mutex):_mutex(mutex)
    {
        pthread_mutex_lock(&mutex._mutex);
    }
    ~CLock()
    {
        pthread_mutex_unlock(&_mutex._mutex);
    }
};

#endif // MAIN_H
