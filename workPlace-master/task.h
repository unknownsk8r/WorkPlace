#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QDate>
#include <QDataStream>

#include "worker.h"

class Task
{
public:
    Task();

    enum class Status {
        InProgress,
        Done
    };

    QString task() const;
    QDateTime startTime() const;
    QDateTime endTime() const;
    //! Возващает указатель на рабочего, выполняющего задание.
    Worker worker() const;
    //! Возвращает указатель на диспетчера, начавшего задание.
    Worker dispatcher() const;
    Status status() const;

    void setStatus(const Status status);
    QString statusToString() const;

    void setTask(const QString&);
    void setTime(const QDateTime& startTime, const QDateTime& endTime);
    void setEmployers(const Worker& dispatcher, const Worker& worker);

    void save(QDataStream& ost) const;
    void load(QDataStream& ist);

private:
    QString t_task;
    //! Время начала выполнения работы
    QDateTime t_startTime;
    //! Срок сдачи работы
    QDateTime t_endTime;
    //! Указатель на рабочего, выполняющего задание.
    Worker t_worker;
    //! Указатель на диспетчера, начавшего задание.
    Worker t_dispatcher;
    //! ...
    Status t_status;
};

inline QDataStream &operator<<(QDataStream &ost, const Task &task)
{
    task.save(ost);
    return ost;
}

inline QDataStream &operator>>(QDataStream &ist, Task &task)
{
    task.load(ist);
    return ist;
}


#endif // TASK_H
