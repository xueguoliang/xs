#include "csqltablemodel.h"

CSqlTableModel::CSqlTableModel(QString tableName, QObject *parent) :
    QSqlTableModel(parent)
{
    setTable(tableName);
    setEditStrategy(QSqlTableModel::OnManualSubmit);
}

bool CSqlTableModel::setFilterAndSelect(QString filter)
{
    this->setFilter(filter);
    return this->select();
}
bool CSqlTableModel::insert(int n, ...)
{
    va_list ap;
    QSqlRecord r = record();
    va_start(ap, n);
    for(int i=0; i<n;++i)
    {
        const char* v = va_arg(ap, const char*);
        r.setValue(i+1, v);
    }
    va_end(ap);

    return insertRecord(-1, r);
}
