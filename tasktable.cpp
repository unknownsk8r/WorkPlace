#include "tasktable.h"

#include <QDate>
#include <stdexcept>
#include <QDebug>
#include <set>

TaskTable::TaskTable(QObject *parent)
    : QAbstractTableModel(parent)
{
}

QVariant TaskTable::headerData(int section, Qt::Orientation orientation, int role) const
{
    // Если требуется текст для отображения...
    if (role == Qt::DisplayRole)
    {
        // Если речь о заголовках столбцов...
        if (orientation == Qt::Horizontal)
        {
            switch (section)
            {
            case 0:
                return tr("Задание");
            case 1:
                return tr("Диспетчер");
            case 2:
                return tr("Рабочий");
            case 3:
                return tr("Время начала работы");
            case 4:
                return tr("Время окончания работы");
            case 5:
                return tr("Статус выполнения");
            }
        }
        // Если речь о заголовках строк...
        else if (orientation == Qt::Vertical)
        {
            // Возвращаем номер строки
            return QString::number(section);
        }
    }
    // Игнорируем все остальные запросы, возвращая пустой QVariant
    return QVariant();
}

int TaskTable::rowCount(const QModelIndex &parent) const
{
    return !parent.isValid() ? tasks.size() : 0;
}

int TaskTable::columnCount(const QModelIndex &parent) const
{
    return !parent.isValid() ? 6 : 0;
}

QVariant TaskTable::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case 0:
            return tasks[index.row()].task();
        case 1:
            return tasks[index.row()].dispatcher().fullName();
        case 2:
            return tasks[index.row()].worker().fullName();
        case 3:
            return tasks[index.row()].startTime().toString();
        case 4:
            return tasks[index.row()].endTime().toString();
        case 5:
            return tasks[index.row()].statusToString();
        }
    }
    // Игнорируем все остальные запросы, возвращая пустой QVariant
    return QVariant();
}

void TaskTable::insert(const Task &task)
{
    beginInsertRows(QModelIndex(), // Индекс родителя, в список потомков которого добавляются строки
                    size(), // Номер первой добавляемой строки
                    size() // Номер последней добавляемой строки
                    );
    // Вставляем заметку в конец вектора mNotes
    tasks.push_back(task);
    endInsertRows();
}

void TaskTable::erase(size_t idx)
{
    beginRemoveRows(QModelIndex(), // Индекс родителя, из списка потомков которого удаляются строки
                    idx, // Номер первой удаляемой строки
                    idx // Номер последней удаляемой строки
                    );
    // Удаляем из вектора элемент с индексом idx
    tasks.erase(std::next(tasks.begin(), idx));
    endRemoveRows();
}

void TaskTable::edit(const QModelIndex idx, Task &currentTask)
{
    tasks[idx.row()].setEmployers(currentTask.dispatcher(), currentTask.worker());
    tasks[idx.row()].setTask(currentTask.task());
    tasks[idx.row()].setTime(currentTask.startTime(), currentTask.endTime());
    tasks[idx.row()].setStatus(currentTask.status());

    emit dataChanged(idx, idx);
}

void TaskTable::save(QDataStream &ost) const
{
    // Цикл по всем заметкам
    for (const Task &element : tasks)
    {
        // Выводим заметку в поток
        ost << element;
        // Если возникла ошибка, запускаем исключительную ситуацию
        if (ost.status() == QDataStream::WriteFailed)
        {
            throw std::runtime_error(tr("сохранение списка задач было прервано.").toStdString());
        }
    }
}

size_t TaskTable::load(QDataStream &ist)
{
    beginResetModel();
    while (!ist.atEnd())
    {
        Task task;
        // Читаем студента из потока.
        ist >> task;
        // Если возникла ошибка, запускаем исключительную ситуацию
        if (ist.status() == QDataStream::ReadCorruptData)
        {
            throw std::runtime_error(tr("запись данных для составления списка задач была прервано.").toStdString());
        }
        // qDebug() << worker->id();

        if (worker->right() == User::Right::Worker)
        {
            if (task.worker() == *worker) { tasks.push_back(task); }
        }
        else
        {
            tasks.push_back(task);
        }

    }
    endResetModel();

    return tasks.size();
}

void TaskTable::deleteTaskByWorker(Worker &worker)
{
    // Для хранения номеров строк создаём STL-контейнер "множество", элементы
    // которого автоматически упорядочиваются по возрастанию
    std::set<int> rows;
    // Проходим по всему списку задач
    for (const auto& element : tasks)
    {
        if (element.worker() == worker || element.dispatcher() == worker)
        {
            rows.insert(&element - &tasks[0]);
        }
    }

    // Обходим множество номеров выбранных строк *по убыванию*, чтобы удаление предыдущих
    // не сбивало нумерацию следующих
    for (auto it = rows.rbegin(); it != rows.rend(); ++it)
    {
        erase(*it);
    }
}

Task &TaskTable::task(QModelIndex index)
{
    return tasks[index.row()];
}

void TaskTable::setWorker(Worker* worker)
{
    this->worker = worker;
}

size_t TaskTable::size() const
{
    return tasks.size();
}
