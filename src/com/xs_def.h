
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
#ifndef __XS_DEF_H__
#define __XS_DEF_H__


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <inttypes.h>
#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>

#include <regex.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
//#include <dirent.h>

#ifndef offsetof
#define offsetof(type, member) ((size_t) &((type *)0)->member)
#endif

#ifndef container_of
#ifdef WIN32
#define container_of(ptr, type, member)                \
            (type *)((char *)ptr - offsetof(type, member))
#else
#define container_of(ptr, type, member) ({            \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetof(type,member) );})
#endif
#endif
#define xs_entry(ptr, type, member) \
    container_of(ptr, type, member)





#endif
#ifdef __cplusplus
}
#endif
