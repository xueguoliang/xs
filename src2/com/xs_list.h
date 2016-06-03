
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
#ifndef __XS_LIST_H__
#define __XS_LIST_H__

typedef struct __xs_list
{
    struct __xs_list* prev;
    struct __xs_list* next;
} xs_list_t;

static inline void xs_list_init(xs_list_t* head)
{
    head->prev = head->next = head;
}

static inline int xs_list_empty(xs_list_t* head)
{
    return head == head->next;
}

static inline void __xs_list_add(xs_list_t* prev, xs_list_t* next, xs_list_t* new1)
{
    assert(new1->next == new1);

    prev->next = new1;
    next->prev = new1;
    new1->next = next;
    new1->prev = prev;
}

static inline void xs_list_add_head(xs_list_t* head, xs_list_t* new1)
{
    __xs_list_add(head, head->next, new1);
}

static inline void xs_list_add_tail(xs_list_t* head, xs_list_t* new1)
{
    __xs_list_add(head->prev, head, new1);
}

static inline void __xs_list_del(xs_list_t* prev, xs_list_t* next)
{
    prev->next = next;
    next->prev = prev;
}

static inline void xs_list_del(xs_list_t* entry)
{
    __xs_list_del(entry->prev, entry->next);
    xs_list_init(entry);
}

static inline void xs_list_del_head(xs_list_t* head)
{
    xs_list_del(head->next);
}

static inline void xs_list_del_tail(xs_list_t* head)
{
    xs_list_del(head->prev);
}

static inline void xs_list_move_head(xs_list_t* head, xs_list_t* node)
{
    xs_list_del(node);
    xs_list_add_head(head, node);
}

static inline void xs_list_move_tail(xs_list_t* head, xs_list_t* node)
{
    xs_list_del(node);
    xs_list_add_tail(head, node);
}

static inline void xs_list_append(xs_list_t* to, xs_list_t* from)
{
    if(xs_list_empty(from))
        return;

    xs_list_t* from_tail = from->prev;
    xs_list_t* from_head = from->next;

    to->prev->next = from_head;
    from_head->prev = to->prev;

    from_tail->next = to;
    to->prev = from_tail;

    xs_list_init(from);
}

#define xs_list_entry xs_entry

#define xs_list_first_entry(ptr, type, member) \
    xs_list_entry((ptr)->next, type, member)

#define list_last_entry(ptr, type, member) \
    xs_list_entry((ptr)->prev, type, member)


#define xs_list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

#define xs_list_for_each_entry(pos, head, member)                \
    for (pos = xs_list_entry((head)->next, typeof(*pos), member);    \
         &pos->member != (head);     \
         pos = xs_list_entry(pos->member.next, typeof(*pos), member))

#endif
#ifdef __cplusplus
}
#endif
