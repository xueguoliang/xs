
#include "xs_cli.h"

xs_tree_t g_cmd_tree = (xs_tree_t){NULL,};

#if 0
int __xs_cmd_tree_cmp(const struct rb_node* pos, const void* ptr)
{
    xs_tree_node_t* node = rb_entry(pos, xs_tree_node_t, node);
    xs_cmd_tree_node_t* cmd_node = xs_entry(node, xs_cmd_tree_node_t, node);
    return strcmp(cmd_node->reg, (char*)ptr);
}
#endif

int __xs_cmd_tree_regex_cmp(const struct rb_node* pos, const void* ptr)
{
    xs_tree_node_t* node = rb_entry(pos, xs_tree_node_t, node);
    xs_cmd_tree_node_t* cmd_node = xs_entry(node, xs_cmd_tree_node_t, node);
    regmatch_t m;
    if(regexec(&cmd_node->regex, (char*)ptr, 1, &m, 0) == 0
            && m.rm_so == 0 && ((char*)ptr)[m.rm_eo] == 0)
        return 0;
    return strcmp(cmd_node->reg, (char*)ptr);
}

xs_cmd_tree_node_t*  __xs_cmd_tree_node_init(char* reg/*, char* pre*/)
{
    xs_cmd_tree_node_t* node = xs_malloc(sizeof(*node));
    node->reg = xs_strdup(reg);
//    node->pre = xs_strdup(pre);
    regcomp(&node->regex, node->reg, REG_EXTENDED);
    rb_init(&node->node.child, __xs_cmd_tree_regex_cmp); 
    rb_init_node(&node->node.node);
    node->end = 0;

    return node;
}

void __xs_cmd_add_root_node()
{
    xs_cmd_tree_node_t* root = 
        __xs_cmd_tree_node_init("root"/*, "root"*/); 

    root->node.parent = NULL;

    g_cmd_tree.root = &root->node;
}


void xs_cmd_reg(/*char* pre1,*/ char* cmd1)
{
    char* cmd = xs_strdup(cmd1);
   // char* pre = xs_strdup(pre1);

    char* sptrc = NULL;
    //char* sptrp = NULL;
    //char* p = strtok_r(pre, " ", &sptrp);
    char* c = strtok_r(cmd, " ", &sptrc);


    xs_tree_node_t* parent;
    xs_tree_node_t* node;
    xs_cmd_tree_node_t* cmd_node;

    if(g_cmd_tree.root == NULL)
    {
        __xs_cmd_add_root_node();
    }

    parent = g_cmd_tree.root;

    while(c)
    {
        node = xs_tree_find(parent, c);
        /* if find, not need add to tree */
        if(node)
        {
            parent = node;
        }
        else
        {
            /* add the node to tree */
            cmd_node = __xs_cmd_tree_node_init(c/*, p*/);
            xs_tree_insert(&g_cmd_tree, parent, &cmd_node->node, cmd_node->reg);
            parent = &cmd_node->node;
        }
        c = strtok_r(NULL, " ", &sptrc);
        //p = strtok_r(NULL, " ", &sptrp);
    }

    cmd_node = xs_entry(parent, xs_cmd_tree_node_t, node);
    cmd_node->end = 1;

    xs_free(cmd);
}

char* xs_cmd_resolv(char* cmd1, int* size)
{
    char* cmd = xs_strdup(cmd1);
    char* bufret = NULL;
    char* sptrc = NULL;
    char* c = strtok_r(cmd, " \t\n", &sptrc);
    xs_tree_node_t* parent = g_cmd_tree.root;
    xs_tree_node_t* node;
    xs_cmd_tree_node_t* cmd_node;

    int index=0;
    xs_model_t* model = xs_model_alloc(256);
        
    while(c)
    {
        node = xs_tree_find(parent, c);
        if(!node)
        {
            printf("cmd near \"%s\" error\n", c);
            goto RET;
        }
        parent = node;
        cmd_node = xs_entry(parent, xs_cmd_tree_node_t, node);
        xs_model_set(model, index++, c);

        c = strtok_r(NULL, " \t\n", &sptrc);
    }
    model->argc = index;

    // for debug
    for(index=0; index<model->argc; index++)
    {
        xs_logd("%s", model->argv[index]);
    }
    xs_logd("index=%d", model->argc);
    // debug end

    cmd_node = xs_entry(parent, xs_cmd_tree_node_t, node);
    if(cmd_node->end == 0)
    {
        printf("need more argument ...\n");
        goto RET;
    }

    bufret = xs_model_to_buf(model, size);
    xs_loge("bufret = %s", bufret+4);

RET:
    xs_free(cmd);
    xs_model_delete(model);
    return bufret;
}

void xs_cmd_tree_del(xs_tree_node_t* node)
{
    xs_cmd_tree_node_t* cmd_node;

    while(!rb_empty(&node->child))
    {
        xs_cmd_tree_del(xs_entry(node->child.rb_node, xs_tree_node_t, node));
    }

    if(node->parent)
    {
        rb_erase(&node->node, &node->parent->child);
    }
    else
    {
        g_cmd_tree.root = NULL;
    }
    
    cmd_node = xs_entry(node, xs_cmd_tree_node_t, node);
    xs_free(cmd_node->reg);
    regfree(&cmd_node->regex);
    xs_free(cmd_node);
}

void xs_cmd_clear()
{
    if(g_cmd_tree.root)
        xs_cmd_tree_del(g_cmd_tree.root);    
}
