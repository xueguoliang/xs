
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

char* xs_popen_with_output(const char* c, ...)
{
    /* get cmd */
    static char buf[2048];
    FILE* f;
    int ret;
    char cmd[1024];
    va_list ap;

    va_start(ap, c);
    vsprintf(cmd, c, ap);
    va_end(ap);

    f = popen(cmd, "r");

    if(!f)
        return NULL;

    ret = fread(buf, 1, 2048, f);
    fclose(f);

    if(ret <= 0)
        return NULL;

    buf[ret-1] = 0;

    return buf;
}

void xs_popen(const char* c, ...)
{
    /* get cmd */
    char cmd[1024];
    va_list ap;
    FILE* f;
    va_start(ap, c);
    vsprintf(cmd, c, ap);
    va_end(ap);

    f = popen(cmd, "r");
    if(!f)
    {
        return;
    }

    fclose(f);
}

int xs_system(const char* c, ...)
{
    /* get cmd */
    char cmd[1024];
    va_list ap;

    va_start(ap, c);
    vsprintf(cmd, c, ap);
    va_end(ap);

    return system(cmd);
}
#ifdef __cplusplus
}
#endif
