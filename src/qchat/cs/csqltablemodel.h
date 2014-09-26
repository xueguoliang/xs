#ifndef CSQLTABLEMODEL_H
#define CSQLTABLEMODEL_H

#include <QSqlTableModel>
#include <QSqlRecord>
class CSqlTableModel : public QSqlTableModel
{
    Q_OBJECT
public:
    explicit CSqlTableModel(QString tableName, QObject *parent = 0);

    bool insert(int n, ...);
    bool setFilterAndSelect(QString filter);

signals:

public slots:

};

#endif // CSQLTABLEMODEL_H
