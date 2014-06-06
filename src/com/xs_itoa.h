
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
#ifndef __XS_ITOA_H__
#define __XS_ITOA_H__

static inline char* xs_itoa_r(int64_t v, char* buf)
{
#ifdef WIN32
    sprintf(buf, "%I64d", (long long int)v);
#else
    sprintf(buf, "%lld", ( long long int)v);
#endif
    return buf;
}

static inline char* xs_itoa(int64_t v)
{
    static char buf[128];
    return xs_itoa_r(v, buf);
}

#endif
#ifdef __cplusplus
}
#endif
