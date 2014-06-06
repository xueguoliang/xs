

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

#if USE_ZLOG
zlog_category_t* __log__global__ = NULL;

int xs_loginit()
{
    int rc;
    zlog_category_t* c;

    rc = zlog_init("zlog.conf");
    if(rc)
    {
        printf("zlog init failed\n");
        return -1;
    }

    c = zlog_get_category("my_cat");
    if(!c)
    {
        printf("get cat failed\n");
        zlog_fini();
        return -2;
    }

    __log__global__ = c;

    return 0;
}
#endif
#ifdef __cplusplus
}
#endif
