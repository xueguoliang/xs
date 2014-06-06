
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
#ifndef __XS_MALLOC_H__
#define __XS_MALLOC_H__


extern void* __xs_malloc(int size, const char* file, int line);
extern void* __xs_zalloc(int size, const char* file, int line);
extern char* __xs_strdup(const char* ss, const char* file, int line);
extern void __xs_free(void* ptr);
#if 0
extern void* __xs_realloc(void* ptr, int size, const char* file, int line);
#endif

#define xs_malloc(size) __xs_malloc(size, __FILE__, __LINE__)
#define xs_zalloc(size) __xs_zalloc(size, __FILE__, __LINE__)
#define xs_strdup(ss) __xs_strdup(ss, __FILE__, __LINE__)
//#define xs_realloc(ptr, _size) __xs_realloc(ptr, _size, __FILE__, __LINE__)
#define xs_free __xs_free

#endif
#ifdef __cplusplus
}
#endif
