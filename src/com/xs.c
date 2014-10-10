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
    // 如果是windows环境下，初始化socket运行环境
    WSADATA wsaData;
    WSAStartup( MAKEWORD( 2, 1 ), &wsaData );
#endif
    // 创建线程私有数据的key，用于保存和获取线程私有数据，这个key用来保存线程池
    // 每个线程有自己独立的线程池，这样实现是为了免锁
    pthread_key_create(&g_thread_key, NULL);
    // 日志初始化，现在基本没用，可以忽略
    xs_loginit();
    // 创建主线程用的内存池
    g_process.mp = g_mp = xs_mempool_create(NULL, 0, 512000); 
    // 将内存池保存到线程私有数据
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


#ifdef __cplusplus
}
#endif
