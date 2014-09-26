

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
#ifndef __XS_CLIS_H__
#define __XS_CLIS_H__

#ifdef __cplusplus
extern "C"{
#endif

#define __XS_CLIS_PORT 9911
#define __xsc_clis "xs"
#define __xsc_init "init"
#define __xsc_show "show"
#define __xsc_memory "mem"
#define __xsc_check "check"

void xs_clis_init();
void xs_clis_fini();
void xs_clis_reg_command(const char* cmd, xs_object_handler_t op);

#ifdef __cplusplus
}
#endif
#endif
