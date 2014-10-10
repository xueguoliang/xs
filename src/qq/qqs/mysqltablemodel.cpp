#include "mysqltablemodel.h"

MySqlTableModel::MySqlTableModel(QString tableName, QObject *parent) :
    QSqlTableModel(parent)
{
    setTable(tableName);
    setEditStrategy(QSqlTableModel::OnManualSubmit);


}
bool MySqlTableModel::insert(QList<QVariant> vars)
{
    QSqlRecord r = record();
    for(int i=0; i<vars.count(); i++)
    {
        // i+1的原因是因为第一列是auto_increment
        r.setValue(i+1, vars.at(i));
    }
    bool ret = this->insertRecord(-1, r);
    if(ret) ret = this->submitAll();
    return ret;
}
bool MySqlTableModel::insert(int argc, ...)
{
    va_list ap;
    va_start(ap, argc);
    QSqlRecord r = record();
    for(int i=0; i<argc; i++)
    {
        QVariant* var = va_arg(ap, QVariant*);
        r.setValue(i+1, *var);
    }
    va_end(ap);

    bool ret = insertRecord(-1, r);
    if(ret)
    {
        ret = submitAll();
    }
    return ret;
}
