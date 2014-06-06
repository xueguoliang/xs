/*
  Red Black Trees
  (C) 1999  Andrea Arcangeli <andrea@suse.de>
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  linux/include/linux/rbtree.h

  To use rbtrees you'll have to implement your own insert and search cores.
  This will avoid us to use callbacks and to drop drammatically performances.
  I know it's not the cleaner way,  but in C (not in C++) to get
  performances and genericity...

  Some example of insert and search follows here. The search is a plain
  normal search over an ordered tree. The insert instead must be implemented
  in two steps: First, the code must insert the element in order as a red leaf
  in the tree, and then the support library function rb_insert_color() must
  be called. Such function will do the not trivial work to rebalance the
  rbtree, if necessary.

-----------------------------------------------------------------------
static inline struct page * rb_search_page_cache(struct inode * inode,
                         unsigned long offset)
{
    struct rb_node * n = inode->i_rb_page_cache.rb_node;
    struct page * page;

    while (n)
    {
        page = rb_entry(n, struct page, rb_page_cache);

        if (offset < page->offset)
            n = n->rb_left;
        else if (offset > page->offset)
            n = n->rb_right;
        else
            return page;
    }
    return NULL;
}

static inline struct page * __rb_insert_page_cache(struct inode * inode,
                           unsigned long offset,
                           struct rb_node * node)
{
    struct rb_node ** p = &inode->i_rb_page_cache.rb_node;
    struct rb_node * parent = NULL;
    struct page * page;

    while (*p)
    {
        parent = *p;
        page = rb_entry(parent, struct page, rb_page_cache);

        if (offset < page->offset)
            p = &(*p)->rb_left;
        else if (offset > page->offset)
            p = &(*p)->rb_right;
        else
            return page;
    }

    rb_link_node(node, parent, p);

    return NULL;
}

static inline struct page * rb_insert_page_cache(struct inode * inode,
                         unsigned long offset,
                         struct rb_node * node)
{
    struct page * ret;
    if ((ret = __rb_insert_page_cache(inode, offset, node)))
        goto out;
    rb_insert_color(node, &inode->i_rb_page_cache);
 out:
    return ret;
}
-----------------------------------------------------------------------
*/
#ifdef __cplusplus
extern "C"{
#endif
#ifndef    _LINUX_RBTREE_H
#define    _LINUX_RBTREE_H
#include <inttypes.h>
#include <stddef.h>

struct rb_node
{
//    unsigned long  rb_parent_color;
    uint32_t color:1;
    uint32_t usrdata:31;

    struct rb_node* rb_parent;
#define    RB_RED        0
#define    RB_BLACK    1
    struct rb_node *rb_right;
    struct rb_node *rb_left;
};
//__attribute__((aligned(sizeof(long))));
    /* The alignment might seem pointless, but allegedly CRIS needs it */

typedef int (*rb_compare_f1)(const struct rb_node* pos, const void* ptr);
struct rb_root
{
    struct rb_node *rb_node;
    rb_compare_f1 cmp;
};

//#define rb_parent(r)   ((struct rb_node *)((r)->rb_parent_color & ~3))
#define rb_parent(r)   (r->rb_parent)
#define rb_color(r)   ((r)->color)
#define rb_is_red(r)   (!rb_color(r))
#define rb_is_black(r) rb_color(r)
#define rb_set_red(r)  do { (r)->color = RB_RED; } while (0)
#define rb_set_black(r)  do { (r)->color=RB_BLACK; } while (0)

static inline void rb_set_parent(struct rb_node *rb, struct rb_node *p)
{
    rb->rb_parent = p;
}
static inline void rb_set_color(struct rb_node *rb, int color)
{
    rb->color = color;
}

#define RB_ROOT    (struct rb_root) { NULL, NULL,}

#define rb_entry xs_entry

#define RB_EMPTY_ROOT(root)    ((root)->rb_node == NULL)
#define RB_EMPTY_NODE(node)    (rb_parent(node) == node)
#define RB_CLEAR_NODE(node)    (rb_set_parent(node, node))

static inline void rb_init_node(struct rb_node *rb)
{
    rb->rb_parent = NULL;
    rb->color = RB_RED;
    rb->rb_right = NULL;
    rb->rb_left = NULL;
    RB_CLEAR_NODE(rb);
}
static inline struct rb_root* rb_create(rb_compare_f1 cmp)
{
    struct rb_root* root = (struct rb_root*)xs_malloc(sizeof(*root));
    root->cmp = cmp;
    root->rb_node = NULL;
    return root;
}
static inline void rb_init(struct rb_root* root, rb_compare_f1 cmp)
{
    root->rb_node = NULL;
    root->cmp = cmp;
}

static inline int rb_empty(struct rb_root* root)
{
    return RB_EMPTY_ROOT(root);
}

extern void rb_insert_color(struct rb_node *, struct rb_root *);
extern void rb_erase(struct rb_node *, struct rb_root *);


extern int rb_insert(struct rb_node*, struct rb_root*, const void* key);
extern struct rb_node* rb_find(struct rb_root* root, const void* key);
typedef void (*rb_augment_f)(struct rb_node *node, void *data);

extern void rb_augment_insert(struct rb_node *node,
                  rb_augment_f func, void *data);
extern struct rb_node *rb_augment_erase_begin(struct rb_node *node);
extern void rb_augment_erase_end(struct rb_node *node,
                 rb_augment_f func, void *data);

/* Find logical next and previous nodes in a tree */
extern struct rb_node *rb_next(const struct rb_node *);
extern struct rb_node *rb_prev(const struct rb_node *);
extern struct rb_node *rb_first(const struct rb_root *);
extern struct rb_node *rb_last(const struct rb_root *);

/* Fast replacement of a single node without remove/rebalance/add/rebalance */
extern void rb_replace_node(struct rb_node *victim, struct rb_node *new1,
                struct rb_root *root);

static inline void rb_link_node(struct rb_node * node, struct rb_node * parent,
                struct rb_node ** rb_link)
{
    node->rb_parent = parent;
    node->rb_left = node->rb_right = NULL;
    node->color = RB_RED;

    *rb_link = node;
}


typedef char* (*rb_get_name_f)(const struct rb_node* node);
void rb_dump(struct rb_root* tree, const char* filename, 
                rb_get_name_f func);


#endif    /* _LINUX_RBTREE_H */
#ifdef __cplusplus
}
#endif
