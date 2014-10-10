

#include "main.h"
#include "mysqltablemodel.h"

CMutex g_mutex;

void handle_send_message(xs_model_t* model, int fd, xs_ctrl_t*)
{
    // 加锁
    CLock unuseLock(g_mutex);

    MySqlTableModel sqlModel("qqchat");
    QString strFilter;
    for(int i=0; i<model->argc; i++)
    {
        xs_logd("***************************%s", model->argv(i));
    }
    strFilter.sprintf("f2='%s'", model->argv(2));

    sqlModel.setFilter(strFilter);

    sqlModel.select();

    if(sqlModel.rowCount() > 0)
    {
        xs_model_t* model = xs_model_create(4);

        model->argv(0) = xs_strdup(__xs_ok);

        qDebug() << sqlModel.data(sqlModel.index(0, 1)).
                    toString().toUtf8().data();
        model->argv(1) = xs_strdup(sqlModel.data(sqlModel.index(0, 1)).
                                   toString().toUtf8().data());

        qDebug() << sqlModel.data(sqlModel.index(0, 2)).
                    toString().toUtf8().data();

        model->argv(2) = xs_strdup(sqlModel.data(sqlModel.index(0, 2)).
                                   toString().toUtf8().data());

        qDebug() << sqlModel.data(sqlModel.index(0, 3)).
                    toString().toUtf8().data();
        model->argv(3) = xs_strdup(sqlModel.data(sqlModel.index(0, 3)).
                                   toString().toUtf8().data());
        xs_model_send_and_close(fd, model);
        // 服务器删除记录
        bool ret = sqlModel.removeRow(0);
        if(ret)
            ret = sqlModel.submitAll();
        if(!ret )
        {
            qDebug()<<"Recv Error: " << sqlModel.lastError().text();

        }
        return;
    }
    xs_model_send_and_close_v(fd, 1, __xs_err);
}

// 转发？不是转发，而是将聊天信息保存在服务器数据库中
void handle_chat(xs_model_t* model, int fd, xs_ctrl_t*)
{
    // 加锁
    CLock unuseLock(g_mutex);
    const char* user = model->argv(2);
    const char* peer = model->argv(3);
    const char* content =model->argv(4);

    MySqlTableModel sqlModel("qqchat");
  //  sqlModel.insert(3, user, peer, content);
    bool ret = sqlModel.insert(QList<QVariant>() <<
                               QVariant(user) << QVariant(peer) << content);
#if 0
    if(ret)
    {
        xs_model_send_and_close_v(fd, 1, __xs_ok);
        return;
    }
    xs_model_send_and_close_v(fd, 2, __xs_err, "insert data base error");
#endif

    // 检查聊天记录表，看看是不是有消息要发给客户端，
    // 如果有，那么不让客户端关闭socket，接续接收别人给他发的消息
    if(ret)
    {
        QString strFilter;
        strFilter.sprintf("f2='%s'", user);
        sqlModel.setFilter(strFilter);
        sqlModel.select();
        if(sqlModel.rowCount() > 0)
            xs_model_send_and_close_v(fd, 2, __xs_ok, "1");
        else
            xs_model_send_and_close_v(fd, 2, __xs_ok, "0");
    }
    else
    {
        xs_model_send_and_close_v(fd, 2, __xs_err, "insert database error");
    }
}

void handle_friend_list(xs_model_t* model, int fd, xs_ctrl_t* )
{
    // 加锁
    CLock unuseLock(g_mutex);
    const char* username = model->argv(2);
    MySqlTableModel sqlModel("qqfriend");
    QString strFilter;
    strFilter.sprintf("f1='%s'", username);
    sqlModel.setFilter(strFilter);
    sqlModel.select();

    int row = sqlModel.rowCount();
    model = xs_model_create(row);
    for(int i=0; i<row; i++)
    {
        QVariant var = sqlModel.data(sqlModel.index(i, 2));
        model->argv(i) = xs_strdup(var.toString().toUtf8().data());
    }

    xs_model_send_and_close(fd, model);
}

void handle_add_friend(xs_model_t* model, int fd, xs_ctrl_t* )
{
    // 加锁
    CLock unuseLock(g_mutex);
    const char* username = model->argv(2);
    const char* friendName = model->argv(3);
    xs_logd("username is %s, friendName=%s", username, friendName);

    MySqlTableModel sqlModel("qquser");
    QString strFilter;
    strFilter.sprintf("f1='%s'", friendName);
    sqlModel.setFilter(strFilter);
    sqlModel.select();
    if(sqlModel.rowCount() != 1)
    {
        xs_logd("can not find friend");
        xs_model_send_and_close_v(fd, 2, __xs_err, "friend not exist");
        return;
    }

    sqlModel.setTable("qqfriend");

    QVariant varUser(username);
    QVariant varFriend(friendName);

    if(!sqlModel.insert(2, &varUser, &varFriend))
    {
        xs_model_send_and_close_v(fd, 2, __xs_err,
                                  sqlModel.lastError().text().toUtf8().data());
        return;
    }
    if(!sqlModel.insert(2, &varFriend, &varUser))
    {
        xs_model_send_and_close_v(fd, 2, __xs_err,
                                  sqlModel.lastError().text().toUtf8().data());
        return;
    }
    xs_model_send_and_close_v(fd, 1, __xs_ok);
}
void handle_login(xs_model_t* model, int fd, xs_ctrl_t* )
{
    // 加锁
    CLock unuseLock(g_mutex);
    QSqlTableModel sqlModel;
    sqlModel.setTable("qquser");
    sqlModel.setEditStrategy(QSqlTableModel::OnManualSubmit);
    QString strFilter;
    strFilter.sprintf("f1='%s' and f2='%s'", model->argv(2), model->argv(3));
    sqlModel.setFilter(strFilter);
    sqlModel.select();
    if(sqlModel.rowCount() == 1)
    {

        xs_model_send_and_close_v(fd, 1, __xs_ok);
        return;
    }


    xs_model_send_and_close_v(fd, 2, __xs_err, "username or password error");
}

void handle_heartbeat(xs_model_t* model, int fd, xs_ctrl_t* )
{
    CLock unuseLock(g_mutex);

    // 表示这个用户还在线
    const char* user = model->argv(2);
    MySqlTableModel sqlModel("qquser");
    QString strFilter = QString().sprintf("f1='%s'", user);
    sqlModel.setFilterAndSelect(strFilter);

    if(sqlModel.rowCount() == 1)
    {
        // 收到心跳包时，要把用户状态改成2
        sqlModel.setData(sqlModel.index(0, 3), 2);
        sqlModel.submitAll();

        // 要检查是否有人给我发消息，如果有，那么告诉客户端来收
        sqlModel.setTable("qqchat");
        QString strFilter;
        strFilter.sprintf("f2='%s'", user);
        sqlModel.setFilterAndSelect(strFilter);
        if(sqlModel.rowCount() > 0)
            xs_model_send_and_close_v(fd, 2, __xs_ok, "1");
        else
            xs_model_send_and_close_v(fd, 2, __xs_ok, "0");
    }
}
void handle_reg(xs_model_t* model, int fd, xs_ctrl_t* ctrl)
{
    CLock unuseLock(g_mutex);
    model = model;
    fd = fd;
    ctrl = ctrl;
    qDebug() << "i am called";

    QSqlTableModel sqlModel;
    sqlModel.setTable("qquser");
    sqlModel.setEditStrategy(QSqlTableModel::OnManualSubmit);

    QSqlRecord record = sqlModel.record();
    record.setValue("f1", model->argv(2));
    record.setValue("f2", model->argv(3));
    record.setValue("f3", 1);

    if(!sqlModel.insertRecord(-1, record))
    {
     //   xs_model_send_block_arg(fd, 2, __xs_err, "insert Error");
        xs_model_send_and_close_v(fd, 2, __xs_err, "insert Error");
        return;
    }
    if(!sqlModel.submitAll())
    {
        xs_model_send_and_close_v(fd, 2, __xs_err, "submit error");
        return;
    }

    // response
    xs_model_send_and_close_v(fd, 1, __xs_ok);
}

void qq_check_user_status(xs_ev_time_t* )
{
    // 检查所有用户
    CLock unuseLock(g_mutex);
    MySqlTableModel sqlModel("qquser");
    sqlModel.setFilterAndSelect("f3>0");

    int count = sqlModel.rowCount();
    if(count > 0)
    {
        for(int i=0; i<count; i++)
        {
            int v = sqlModel.data(sqlModel.index(i, 3)).toInt();
            v -= 1;
            sqlModel.setData(sqlModel.index(i, 3), QVariant(v));
        }
        sqlModel.submitAll();
    }
}

int main(int argc, char* argv[])
{
    xs_server_init(4, NULL, argc, argv);

    // 连接数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setUserName("root");
    db.setPassword("iamxgl");
    db.setDatabaseName("qq");
    db.setHostName("localhost");
    db.open();

    // 创建一个Ctrl对象
    xs_ctrl_t* ctrl = xs_ctrl_create(__xs_qq_port, NULL, NULL);
    // 在Ctrl中注册处理函数
    xs_ctrl_reg_handler(ctrl, __xsc_userman, __xsc_reg, handle_reg);
    xs_ctrl_reg_handler(ctrl, __xsc_userman, __xsc_login, handle_login);
    xs_ctrl_reg_handler(ctrl, __xsc_userman, __xsc_add_friend, handle_add_friend);
    xs_ctrl_reg_handler(ctrl, __xsc_userman, __xsc_friendlist, handle_friend_list);
    xs_ctrl_reg_handler(ctrl, __xsc_userman, __xsc_heartbeat, handle_heartbeat);

    xs_ctrl_reg_handler(ctrl, __xsc_chat, __xsc_chat, handle_chat);
    xs_ctrl_reg_handler(ctrl, __xsc_chat, __xsc_recv_msg, handle_send_message);

    xs_ev_add_time_ev(10*1000, qq_check_user_status, NULL);

    int ret = xs_server_run();

    xs_server_fini();
    return ret;
}
