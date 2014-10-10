#ifndef MYSQLTABLEMODEL_H
#define MYSQLTABLEMODEL_H

#include "main.h"

class MySqlTableModel : public QSqlTableModel
{
    Q_OBJECT
public:
    explicit MySqlTableModel(QString tableName, QObject *parent = 0);
    bool insert(int argc, ...);
    bool insert(QList<QVariant> vars);
    bool setFilterAndSelect(QString filter)
    {
        setFilter(filter);
        return select();
    }

signals:

public slots:

};

#endif // MYSQLTABLEMODEL_H
