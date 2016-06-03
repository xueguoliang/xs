#include "../com/xs.h"
#include "xs_db.h"
#include <mysql/mysql.h>

static MYSQL conn;
void xs_db_init()
{
    mysql_init(&conn);
    MYSQL* ret = mysql_real_connect(&conn, "localhost", "root", "iamxgl", "xstest", 0, NULL, 0);
    if(ret == NULL)
    {
        xs_err("connect database error: %s\n", mysql_error(&conn));
        return;
    }
    xs_dbg("connect database success\n");
}

void xs_db_fini()
{
    mysql_close(&conn);
}

int xs_db_add_record(char *data, int size)
{
    size = size;
    char sql[2048];
    sprintf(sql, "insert into xstest values(%d, '%s')", (int)getpid(), data);
    int ret = mysql_query(&conn, sql);
    if(ret != 0)
    {
        xs_err("insert error %s\n", mysql_error(&conn));
    }
    return 0;
}


