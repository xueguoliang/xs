
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
#ifndef __XS_ATOMIC_H__
#define __XS_ATOMIC_H__

/*
 * type __sync_fetch_and_add(type* ptr, type value)
 * type __sync_fetch_and_sub(type* ptr, type value)
 * type __sync_fetch_and_or (type* ptr, type value)
 * type __sync_fetch_and_and(type* ptr, type value)
 * type __sync_fetch_and_xor(type* ptr, type value)
 * type __sync_fetch_add_nand(type* ptr, type value)
 *
 * */

#define xs_atomic_get(ptr) __sync_fetch_and_add(ptr, 0)
#define xs_atomic_inc(ptr) __sync_fetch_and_add(ptr, 1)
#define xs_atomic_dec(ptr) __sync_fetch_and_sub(ptr, 1)
#define xs_atomic_zero(ptr) __sync_fetch_and_and(ptr, 0)

#endif
#ifdef __cplusplus
}
#endif
