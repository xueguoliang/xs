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



#ifndef __XS_H__
#define __XS_H__

#include "xs_net.h"
#include "xs_def.h"
#include "xs_log.h"
#include "xs_base64.h"
#include "xs_atomic.h"
#include "xs_list.h"
#include "xs_mempool.h"
#include "xs_malloc.h"
#include "xs_util.h"
#include "xs_heap.h"
#include "xs_sort.h"
#include "xs_rbtree.h"
#include "xs_popen.h"
#include "xs_stat.h"
#include "xs_itoa.h"
#include "xs_dict.h"
#include "xs_config.h"
#include "xs_tree.h"
#include "xs_ev.h"
#include "xs_sock.h"
#include "xs_aio.h"
#include "xs_aio_block.h"
#include "xs_hash.h"
#include "xs_model.h"
#include "xs_model_block.h"
#include "xs_object.h"
#include "xs_ctrl.h"
#include "xs_server.h"
#include "xs_vec.h"
#include "xs_md5.h"
#include "xs_clis.h"


/* XS库初始化函数，xs_init是初始化主线程运行环境的，用于客户端
 * 服务器的初始化应该用xs_server_init */
void xs_init();
void xs_fini();


#endif

#ifdef __cplusplus
}
#endif
