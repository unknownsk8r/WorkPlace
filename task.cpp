#include "task.h"

Task::Task()
{
    this->t_status = Status::InProgress;
}

QString Task::task() const
{
    return t_task;
}

QDateTime Task::startTime() const
{
    return t_startTime;
}

QDateTime Task::endTime() const
{
    return t_endTime;
}

Worker Task::worker() const
{
    return t_worker;
}

Worker Task::dispatcher() const
{
    return t_dispatcher;
}

Task::Status Task::status() const
{
    return t_status;
}

void Task::setStatus(const Status status)
{
    t_status = status;
}

QString Task::statusToString() const
{
    switch (t_status) {
    case Status::InProgress:
        return "In progress";
    case Status::Done:
        return "Ready";
    }
}

void Task::setTask(const QString &task)
{
    this->t_task = task;
}

void Task::setTime(const QDateTime &startTime, const QDateTime &endTime)
{
    this->t_startTime = startTime;
    this->t_endTime = endTime;
}

void Task::setEmployers(const Worker &dispatcher, const Worker &worker)
{
    this->t_dispatcher = dispatcher;
    this->t_worker = worker;
}

void Task::save(QDataStream &ost) const
{
    ost << t_task << t_startTime << t_endTime << t_worker << t_dispatcher;

    // Так как режима всего два, используем тернатрный оператор, а не switch, что более
    // "правильно".
    (t_status == Task::Status::InProgress) ? ost << 0 : ost << 1;
}

void Task::load(QDataStream &ist)
{
    int statusValue;
    ist >> t_task >> t_startTime >> t_endTime >> t_worker >> t_dispatcher >> statusValue;

    (statusValue == 0) ? this->t_status = Task::Status::InProgress : this->t_status = Task::Status::Done;
//    if (statusValue == 0)           { this->t_status = Task::Status::InProgress; }
//    else if (statusValue == 1)      { this->t_status = Task::Status::Done; }
}
