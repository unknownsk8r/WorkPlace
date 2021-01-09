#ifndef WORKERLIST_H
#define WORKERLIST_H

#include "worker.h"

#include <QAbstractTableModel>
#include <QVector>
#include <QString>
#include <memory>

class WorkerList : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit WorkerList(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    //! Вставляет работника в таблицу.
    void insert(const Worker&);
    void erase(size_t);
    void edit(const QModelIndex, Worker&);

    void save(QDataStream& ost) const;
    size_t load(QDataStream& ist);

    bool haveWorker(const Worker* worker);
    Worker &worker(const QModelIndex index);
    void setMode(bool);

private:
    size_t size() const;
    bool mode = true;
    QVector<Worker> workers;
};

inline QDataStream &operator<<(QDataStream &ost, const WorkerList &workList)
{
    workList.save(ost);
    return ost;
}

inline QDataStream &operator>>(QDataStream &ist, WorkerList &workList)
{
    workList.load(ist);
    return ist;
}

#endif // WORKERLIST_H
