/*
  Red Black Trees
  (C) 1999  Andrea Arcangeli <andrea@suse.de>
  (C) 2002  David Woodhouse <dwmw2@infradead.org>
  
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

  linux/lib/rbtree.c
*/
#ifdef __cplusplus
extern "C"{
#endif

#include "xs.h"
#ifdef WIN32
#else
static void rb_add_node(FILE* fstream, 
                struct rb_node* node,
                rb_get_name_f func)
{
    if(node == NULL)
        return;

    char* debug = func(node);
    fprintf(fstream, "node_%s[label=\"<f0> |<f1> %s|<f2> |<f3> %s\"];\n", debug,
                            debug, rb_is_red(node) ? "red":"black");
    xs_free(debug);

    rb_add_node(fstream, node->rb_left, func);
    rb_add_node(fstream, node->rb_right, func);
}

static void rb_add_line(FILE* fstream, 
                struct rb_node* node,
                rb_get_name_f func)
{
    if(node == NULL)
        return;

    char* debug = func(node);
    char* child;
    if(node->rb_left)
    {
        child = func(node->rb_left);
        fprintf(fstream, "\"node_%s\":f0 -> \"node_%s\":f1;\n", debug,
                                    child);
    }
    if(node->rb_right)
    {
        child = func(node->rb_right);
        fprintf(fstream, "\"node_%s\":f2 -> \"node_%s\":f1;\n", debug,
                                            child);
    }
    xs_free(debug);
    rb_add_line(fstream, node->rb_left, func);
    rb_add_line(fstream, node->rb_right, func);
}



void rb_dump(struct rb_root* root, 
                const char* filename,
                rb_get_name_f func)
{
    FILE* fstream = fopen(filename, "w");
    char out_cmd[256];
    int ret;
    if(fstream == NULL)
    {
        return;
    }

    fprintf(fstream, "digraph g{\n");
    fprintf(fstream, "node [shape = record, height=.1];\n");
    rb_add_node(fstream, root->rb_node, func);
    rb_add_line(fstream, root->rb_node, func);
    fprintf(fstream, "}\n");
    fclose(fstream);
    sprintf(out_cmd, "dot -T\"jpg\" -o %s.jpg %s", filename, filename);
    ret = system(out_cmd);
    if(ret == 0)
        unlink(filename);
}
#endif

static void __rb_rotate_left(struct rb_node *node, struct rb_root *root)
{
    struct rb_node *right = node->rb_right;
    struct rb_node *parent = rb_parent(node);

    if ((node->rb_right = right->rb_left))
        rb_set_parent(right->rb_left, node);
    right->rb_left = node;

    rb_set_parent(right, parent);

    if (parent)
    {
        if (node == parent->rb_left)
            parent->rb_left = right;
        else
            parent->rb_right = right;
    }
    else
        root->rb_node = right;
    rb_set_parent(node, right);
}

static void __rb_rotate_right(struct rb_node *node, struct rb_root *root)
{
    struct rb_node *left = node->rb_left;
    struct rb_node *parent = rb_parent(node);

    if ((node->rb_left = left->rb_right))
        rb_set_parent(left->rb_right, node);
    left->rb_right = node;

    rb_set_parent(left, parent);

    if (parent)
    {
        if (node == parent->rb_right)
            parent->rb_right = left;
        else
            parent->rb_left = left;
    }
    else
        root->rb_node = left;
    rb_set_parent(node, left);
}

void rb_insert_color(struct rb_node *node, struct rb_root *root)
{
    struct rb_node *parent, *gparent;

    while ((parent = rb_parent(node)) && rb_is_red(parent))
    {
        gparent = rb_parent(parent);

        if (parent == gparent->rb_left)
        {
            {
                register struct rb_node *uncle = gparent->rb_right;
                if (uncle && rb_is_red(uncle))
                {
                    rb_set_black(uncle);
                    rb_set_black(parent);
                    rb_set_red(gparent);
                    node = gparent;
                    continue;
                }
            }

            if (parent->rb_right == node)
            {
                register struct rb_node *tmp;
                __rb_rotate_left(parent, root);
                tmp = parent;
                parent = node;
                node = tmp;
            }

            rb_set_black(parent);
            rb_set_red(gparent);
            __rb_rotate_right(gparent, root);
        } else {
            {
                register struct rb_node *uncle = gparent->rb_left;
                if (uncle && rb_is_red(uncle))
                {
                    rb_set_black(uncle);
                    rb_set_black(parent);
                    rb_set_red(gparent);
                    node = gparent;
                    continue;
                }
            }

            if (parent->rb_left == node)
            {
                register struct rb_node *tmp;
                __rb_rotate_right(parent, root);
                tmp = parent;
                parent = node;
                node = tmp;
            }

            rb_set_black(parent);
            rb_set_red(gparent);
            __rb_rotate_left(gparent, root);
        }
    }

    rb_set_black(root->rb_node);
}
//EXPORT_SYMBOL(rb_insert_color);

static void __rb_erase_color(struct rb_node *node, struct rb_node *parent,
                 struct rb_root *root)
{
    struct rb_node *other;

    while ((!node || rb_is_black(node)) && node != root->rb_node)
    {
        if (parent->rb_left == node)
        {
            other = parent->rb_right;
            if (rb_is_red(other))
            {
                rb_set_black(other);
                rb_set_red(parent);
                __rb_rotate_left(parent, root);
                other = parent->rb_right;
            }
            if ((!other->rb_left || rb_is_black(other->rb_left)) &&
                (!other->rb_right || rb_is_black(other->rb_right)))
            {
                rb_set_red(other);
                node = parent;
                parent = rb_parent(node);
            }
            else
            {
                if (!other->rb_right || rb_is_black(other->rb_right))
                {
                    rb_set_black(other->rb_left);
                    rb_set_red(other);
                    __rb_rotate_right(other, root);
                    other = parent->rb_right;
                }
                rb_set_color(other, rb_color(parent));
                rb_set_black(parent);
                rb_set_black(other->rb_right);
                __rb_rotate_left(parent, root);
                node = root->rb_node;
                break;
            }
        }
        else
        {
            other = parent->rb_left;
            if (rb_is_red(other))
            {
                rb_set_black(other);
                rb_set_red(parent);
                __rb_rotate_right(parent, root);
                other = parent->rb_left;
            }
            if ((!other->rb_left || rb_is_black(other->rb_left)) &&
                (!other->rb_right || rb_is_black(other->rb_right)))
            {
                rb_set_red(other);
                node = parent;
                parent = rb_parent(node);
            }
            else
            {
                if (!other->rb_left || rb_is_black(other->rb_left))
                {
                    rb_set_black(other->rb_right);
                    rb_set_red(other);
                    __rb_rotate_left(other, root);
                    other = parent->rb_left;
                }
                rb_set_color(other, rb_color(parent));
                rb_set_black(parent);
                rb_set_black(other->rb_left);
                __rb_rotate_right(parent, root);
                node = root->rb_node;
                break;
            }
        }
    }
    if (node)
        rb_set_black(node);
}

void rb_erase(struct rb_node *node, struct rb_root *root)
{
    struct rb_node *child, *parent;
    int color;

    if (!node->rb_left)
        child = node->rb_right;
    else if (!node->rb_right)
        child = node->rb_left;
    else
    {
        struct rb_node *old = node, *left;

        node = node->rb_right;
        while ((left = node->rb_left) != NULL)
            node = left;

        if (rb_parent(old)) {
            if (rb_parent(old)->rb_left == old)
                rb_parent(old)->rb_left = node;
            else
                rb_parent(old)->rb_right = node;
        } else
            root->rb_node = node;

        child = node->rb_right;
        parent = rb_parent(node);
        color = rb_color(node);

        if (parent == old) {
            parent = node;
        } else {
            if (child)
                rb_set_parent(child, parent);
            parent->rb_left = child;

            node->rb_right = old->rb_right;
            rb_set_parent(old->rb_right, node);
        }

        node->rb_parent = old->rb_parent;
        node->color = old->color;
        node->rb_left = old->rb_left;
        rb_set_parent(old->rb_left, node);

        goto color;
    }

    parent = rb_parent(node);
    color = rb_color(node);

    if (child)
        rb_set_parent(child, parent);
    if (parent)
    {
        if (parent->rb_left == node)
            parent->rb_left = child;
        else
            parent->rb_right = child;
    }
    else
        root->rb_node = child;

 color:
    if (color == RB_BLACK)
        __rb_erase_color(child, parent, root);
}
//EXPORT_SYMBOL(rb_erase);
struct rb_node* rb_find(struct rb_root *root, const void *key)
{
    int ret;
    struct rb_node* node = root->rb_node;
    while(node)
    {
        ret = root->cmp(node, key);

        if(ret == 0)
            return node;
        if(ret > 0)
            node = node->rb_left;
        else if(ret < 0)
            node = node->rb_right;
    }

    return NULL;
}

int rb_insert(struct rb_node *node, struct rb_root *root, const void* key)
{
    struct rb_node ** p = &root->rb_node;
    struct rb_node * parent = NULL;
    int ret;

    while (*p)
    {
        parent = *p;
        ret = root->cmp(parent, key);

        if (ret > 0)
            p = &(*p)->rb_left;
        else if (ret < 0)
            p = &(*p)->rb_right;
        else
            return -1;
    }

    rb_link_node(node, parent, p);
    rb_insert_color(node, root);
    return 0;
}

static void rb_augment_path(struct rb_node *node, rb_augment_f func, void *data)
{
    struct rb_node *parent;

up:
    func(node, data);
    parent = rb_parent(node);
    if (!parent)
        return;

    if (node == parent->rb_left && parent->rb_right)
        func(parent->rb_right, data);
    else if (parent->rb_left)
        func(parent->rb_left, data);

    node = parent;
    goto up;
}

/*
 * after inserting @node into the tree, update the tree to account for
 * both the new entry and any damage done by rebalance
 */
void rb_augment_insert(struct rb_node *node, rb_augment_f func, void *data)
{
    if (node->rb_left)
        node = node->rb_left;
    else if (node->rb_right)
        node = node->rb_right;

    rb_augment_path(node, func, data);
}
//EXPORT_SYMBOL(rb_augment_insert);

/*
 * before removing the node, find the deepest node on the rebalance path
 * that will still be there after @node gets removed
 */
struct rb_node *rb_augment_erase_begin(struct rb_node *node)
{
    struct rb_node *deepest;

    if (!node->rb_right && !node->rb_left)
        deepest = rb_parent(node);
    else if (!node->rb_right)
        deepest = node->rb_left;
    else if (!node->rb_left)
        deepest = node->rb_right;
    else {
        deepest = rb_next(node);
        if (deepest->rb_right)
            deepest = deepest->rb_right;
        else if (rb_parent(deepest) != node)
            deepest = rb_parent(deepest);
    }

    return deepest;
}
//EXPORT_SYMBOL(rb_augment_erase_begin);

/*
 * after removal, update the tree to account for the removed entry
 * and any rebalance damage.
 */
void rb_augment_erase_end(struct rb_node *node, rb_augment_f func, void *data)
{
    if (node)
        rb_augment_path(node, func, data);
}
//EXPORT_SYMBOL(rb_augment_erase_end);

/*
 * This function returns the first node (in sort order) of the tree.
 */
struct rb_node *rb_first(const struct rb_root *root)
{
    struct rb_node    *n;

    n = root->rb_node;
    if (!n)
        return NULL;
    while (n->rb_left)
        n = n->rb_left;
    return n;
}
//EXPORT_SYMBOL(rb_first);

struct rb_node *rb_last(const struct rb_root *root)
{
    struct rb_node    *n;

    n = root->rb_node;
    if (!n)
        return NULL;
    while (n->rb_right)
        n = n->rb_right;
    return n;
}
//EXPORT_SYMBOL(rb_last);

struct rb_node *rb_next(const struct rb_node *node)
{
    struct rb_node *parent;

    if (rb_parent(node) == node)
        return NULL;

    /* If we have a right-hand child, go down and then left as far
       as we can. */
    if (node->rb_right) {
        node = node->rb_right;
        while (node->rb_left)
            node=node->rb_left;
        return (struct rb_node *)node;
    }

    /* No right-hand children.  Everything down and left is
       smaller than us, so any 'next' node must be in the general
       direction of our parent. Go up the tree; any time the
       ancestor is a right-hand child of its parent, keep going
       up. First time it's a left-hand child of its parent, said
       parent is our 'next' node. */
    while ((parent = rb_parent(node)) && node == parent->rb_right)
        node = parent;

    return parent;
}
//EXPORT_SYMBOL(rb_next);

struct rb_node *rb_prev(const struct rb_node *node)
{
    struct rb_node *parent;

    if (rb_parent(node) == node)
        return NULL;

    /* If we have a left-hand child, go down and then right as far
       as we can. */
    if (node->rb_left) {
        node = node->rb_left;
        while (node->rb_right)
            node=node->rb_right;
        return (struct rb_node *)node;
    }

    /* No left-hand children. Go up till we find an ancestor which
       is a right-hand child of its parent */
    while ((parent = rb_parent(node)) && node == parent->rb_left)
        node = parent;

    return parent;
}
//EXPORT_SYMBOL(rb_prev);

void rb_replace_node(struct rb_node *victim, struct rb_node *new,
             struct rb_root *root)
{
    struct rb_node *parent = rb_parent(victim);

    /* Set the surrounding nodes to point to the replacement */
    if (parent) {
        if (victim == parent->rb_left)
            parent->rb_left = new;
        else
            parent->rb_right = new;
    } else {
        root->rb_node = new;
    }
    if (victim->rb_left)
        rb_set_parent(victim->rb_left, new);
    if (victim->rb_right)
        rb_set_parent(victim->rb_right, new);

    /* Copy the pointers/colour from the victim to the replacement */
    *new = *victim;
}
//EXPORT_SYMBOL(rb_replace_node);
#ifdef __cplusplus
}
#endif
