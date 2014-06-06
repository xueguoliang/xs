

#ifndef __XS_CMD_TREE_H__
#define __XS_CMD_TREE_H__

typedef struct xs_cmd_tree_node_t
{
    char*               reg; /* 命令或者正则表达式，用来判断命令或者tab补全 */
//    char*               pre;
    regex_t             regex;
    xs_tree_node_t      node;
    int                 end;
} xs_cmd_tree_node_t;

extern xs_tree_t g_cmd_tree;

void xs_cmd_reg(/*char* pre, */char* cmd);
void xs_cmd_clear();
char* xs_cmd_resolv(char* cmd, int* size);

#endif
