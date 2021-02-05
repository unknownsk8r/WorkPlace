#ifndef TASKTABLE_H
#define TASKTABLE_H

#include <QAbstractTableModel>
#include <QDataStream>
#include <QVector>
#include <memory>

#include "task.h"

class TaskTable : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit TaskTable(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    //! Вставляет студента в таблицу.
    void insert(const Task&);
    void erase(size_t);
    void edit(const QModelIndex, Task&);

    void save(QDataStream& ost) const;
    size_t load(QDataStream& ist);

    void deleteTaskByWorker(Worker &worker);
    Task &task(QModelIndex index);
    void setWorker(Worker*);
    size_t size() const;

private:
    Worker* worker;
    QVector<Task> tasks;
};

inline QDataStream &operator<<(QDataStream &ost, const TaskTable &taskTable)
{
    taskTable.save(ost);
    return ost;
}

inline QDataStream &operator>>(QDataStream &ist, TaskTable &taskTable)
{
    taskTable.load(ist);
    return ist;
}

#endif // TASKTABLE_H
