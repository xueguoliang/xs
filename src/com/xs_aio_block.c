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

int xs_aio_send_block(int fd, const char* buf, int size, int timeout)
{
    int ret;
    size = htonl(size);
    ret = xs_sock_send_block(fd, &size, 4, timeout);
    if(ret == 4)
    {
        size = ntohl(size);
        ret = xs_sock_send_block(fd, buf, size, timeout);
    }

    return ret;
}

int xs_aio_recv_block(int fd, char** buf, int* size, int timeout)
{
    int ret;
    ret = xs_sock_recv_block(fd, size, 4, timeout);
    if(ret == 4)
    {
        *size = ntohl(*size);
        *buf = malloc(*size + 1);
        (*buf)[*size] = 0;
        ret = xs_sock_recv_block(fd, *buf, *size, timeout);
    }
    return ret;
}

#ifdef __cplusplus
}
#endif
