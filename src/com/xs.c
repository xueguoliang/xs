/*
 * This file is part of the xs Library.
 *
 * Copyright (C) 2011 by Guoliang Xue <xueguoliang@gmail.com>
 *
 * The xs Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The xs Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the xs Library. If not, see <http://www.gnu.org/licenses/>.
 */
#ifdef __cplusplus
extern "C"{
#endif
#include "xs.h"
pthread_key_t g_thread_key;
xs_mempool_t* g_mp;

void xs_init()
{
#ifdef WIN32
    WSADATA wsaData;
    WSAStartup( MAKEWORD( 2, 1 ), &wsaData );
#endif
    pthread_key_create(&g_thread_key, NULL);
    xs_loginit();
    g_process.mp = g_mp = xs_mempool_create(NULL, 0, 512000); 
    pthread_setspecific(g_thread_key, g_mp);


}

void xs_fini()
{

#ifdef XS_USE_MEMPOOL
    xs_mempool_destroy(g_mp);
#endif
    xs_logfini();
    pthread_key_delete(g_thread_key);
#ifdef WIN32
    WSACleanup();
#endif
}

char* xs_gen_session_name(int n)
{
    time_t t = time(NULL);    
    char buf[128];
    sprintf(buf, "%lld|%d", (long long int)t, n);
    return xs_strdup(buf);
}

#ifdef __cplusplus
}
#endif
