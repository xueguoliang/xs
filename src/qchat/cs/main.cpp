
#include "main.h"
void qchat_reg(xs_model_t* model, int fd, struct xs_ctrl_t* ctrl)
{
    model = model;
    fd = fd;
    ctrl = ctrl;

    QSqlTableModel dbModel;
    dbModel.setTable("TUSER");
    dbModel.setEditStrategy(QSqlTableModel::OnManualSubmit);
    QSqlRecord record = dbModel.record();
    record.setValue("F2", model->argv(2));
    record.setValue("F3", model->argv(3));
    if(!dbModel.insertRecord(-1, record))
    {
        xs_model_send_and_close_v(fd, 2, __xs_err,
                                  "insert data error");
                return;
    }

    if(!dbModel.submitAll())
    {
        xs_model_send_and_close_v(fd, 2, __xs_err,
                                  dbModel.database().lastError().text().toUtf8().data());
        return;
    }

    xs_model_send_and_close_v(fd, 1, __xs_ok);
}

void qchat_login(xs_model_t* model, int fd, struct xs_ctrl_t* ctrl)
{
    model = model;
    fd = fd;
    ctrl = ctrl;

    QSqlTableModel dbModel ;
    dbModel.setTable("TUSER");
    dbModel.setEditStrategy(QSqlTableModel::OnManualSubmit);
    QString filter;
    filter.sprintf("F2='%s' and F3='%s'", model->argv(2), model->argv(3));
    xs_logd("filter is %s", filter.toUtf8().data());
    dbModel.setFilter(filter);
    dbModel.select();

    if(dbModel.rowCount() == 1)
    {
        dbModel.setData(dbModel.index(0, 3), QVariant("online"));
        dbModel.submitAll();
        xs_model_send_and_close_v(fd, 1, __xs_ok);
        return;
    }

    xs_logd("record count is %d", dbModel.rowCount());
    xs_model_send_and_close_v(fd, 2, __xs_err, "login error");
}

void qchat_get_friend_list(xs_model_t* model, int fd, struct xs_ctrl_t* ctrl)
{
    ctrl = ctrl;
    CSqlTableModel shipModel("TSHIP");
    QString strFilter;
    strFilter.sprintf("F2='%s' and F4='friend'", model->argv(2));
    shipModel.setFilterAndSelect(strFilter);

    model = xs_model_create(shipModel.rowCount());
    for(int i=0; i<shipModel.rowCount(); ++i)
    {
        QString str = shipModel.data(shipModel.index(i, 2)).toString();
        model->argv(i) = xs_strdup(str.toUtf8().data());
    }

    xs_model_send_and_close(fd, model);
}

void qchat_add_friend(xs_model_t* model, int fd, struct xs_ctrl_t* ctrl)
{
    ctrl = ctrl;
    xs_logd("qchat_add_friend called***************************");

    CSqlTableModel userModel("TUSER");

    QString strFilter;
    strFilter.sprintf("F2='%s'", model->argv(3));
    xs_logd("filter is %s", strFilter.toUtf8().data());
    userModel.setFilterAndSelect(strFilter);

    if(userModel.rowCount() == 0)
    {
        xs_model_send_and_close_v(fd, 2, __xs_err, "no user");
        xs_logd("error: cannot find user");
        return;
    }

    xs_logd("userModel count is %d", userModel.rowCount());

    CSqlTableModel relationShipModel("TSHIP");

    if(!relationShipModel.insert(3, model->argv(2), model->argv(3), "friend"))
    {
        xs_model_send_and_close_v(fd, 2, __xs_err, "can not add data1");
        xs_logd("error: cannot add data1");
        return;
    }

    if(!relationShipModel.insert(3, model->argv(3), model->argv(2), "friend"))
    {
        xs_model_send_and_close_v(fd, 2, __xs_err, "can not add data2");
        xs_logd("error: cannot add data2");
        return;
    }

    if(!relationShipModel.submitAll())
    {
        xs_model_send_and_close_v(fd, 2, __xs_err, "submitAll error");
        xs_logd("error: submit all error");
        return;
    }
    xs_model_send_and_close_v(fd, 1, __xs_ok);
}

int main(int argc, char *argv[])
{
  //  QCoreApplication a(argc, argv);
    xs_server_init(4, NULL, argc, argv);
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName("chat");
    db.setHostName("localhost");
    db.setUserName("root");
    db.setPassword("iamxgl");
    if(db.open())
    {
        xs_logd("open database OK");
    }
    else
    {
        xs_logd("open database ERR");
        return 0;
    }

    xs_ctrl_t* ctrl = xs_ctrl_create(__XS_CHAT_PORT, NULL, NULL);
    xs_ctrl_reg_handler(ctrl, __xsc_userman, __xsc_reg, qchat_reg);
    xs_ctrl_reg_handler(ctrl, __xsc_userman, __xsc_login, qchat_login);
    xs_ctrl_reg_handler(ctrl, __xsc_userman, __xsc_add_friend, qchat_add_friend);
    xs_ctrl_reg_handler(ctrl, __xsc_userman, __xsc_friend_list, qchat_get_friend_list);

    return xs_server_run();
}
