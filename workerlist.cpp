#include "workerlist.h"

#include <stdexcept>
#include <exception>

WorkerList::WorkerList(QObject *parent)
    : QAbstractTableModel(parent)
{

}

QVariant WorkerList::headerData(int section, Qt::Orientation orientation, int role) const
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
                return tr("ID");
            case 1:
                return tr("Фамилия");
            case 2:
                return tr("Имя");
            case 3:
                return tr("Отчество");
            case 4:
                return tr("Права");
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

int WorkerList::rowCount(const QModelIndex &parent) const
{
    return !parent.isValid() ? workers.size() : 0;
}

int WorkerList::columnCount(const QModelIndex &parent) const
{
    return !parent.isValid() ? 4 : 0;
}

QVariant WorkerList::data(const QModelIndex &index, int role) const
{
    // Если требуется текст для отображения...
    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case 0:
            return workers[index.row()].secondName();
        case 1:
            return workers[index.row()].firstName();
        case 2:
            return workers[index.row()].middleName();
        case 3:
            return workers[index.row()].rightToString();
        }
    }

    // Игнорируем все остальные запросы, возвращая пустой QVariant
    return QVariant();
}

void WorkerList::insert(const Worker &worker)
{
    if (haveWorker(&worker))
    {
        throw std::logic_error("Пользователь с таким логином уже существует");
    }

    beginInsertRows(QModelIndex(), // Индекс родителя, в список потомков которого добавляются строки
                    size(), // Номер первой добавляемой строки
                    size() // Номер последней добавляемой строки
                    );
    workers.push_back(worker);
    endInsertRows();
}

void WorkerList::erase(size_t idx)
{
    beginRemoveRows(QModelIndex(), // Индекс родителя, из списка потомков которого удаляются строки
                    idx, // Номер первой удаляемой строки
                    idx // Номер последней удаляемой строки
                    );
    // Удаляем из вектора элемент с индексом idx
    workers.erase(std::next(workers.begin(), idx));
    endRemoveRows();
}

void WorkerList::edit(const QModelIndex idx, Worker &worker)
{
    workers[idx.row()].setPersonData(worker.firstName(), worker.secondName(), worker.middleName());
    workers[idx.row()].setUserData(worker.login(), worker.password(), worker.right());

    emit dataChanged(idx, idx);
}

void WorkerList::save(QDataStream &ost) const
{
    // Цикл по всем заметкам
    for (const Worker &element : workers)
    {
        // Выводим заметку в поток
        ost << element;
        // Если возникла ошибка, запускаем исключительную ситуацию
        if (ost.status() == QDataStream::WriteFailed)
        {
            throw std::runtime_error(tr("запись данных о рабочих в поток была прервана.").toStdString());
        }
    }
}

size_t WorkerList::load(QDataStream &ist)
{
    beginResetModel();
    // Удаляем все заметки
    workers.clear();
    // Пока в потоке есть данные
    while (!ist.atEnd())
    {
        Worker worker;
        // Читаем студента из потока.
        ist >> worker;
        // Если возникла ошибка, запускаем исключительную ситуацию
        if (ist.status() == QDataStream::ReadCorruptData)
        {
            throw std::runtime_error(tr("чтение данных для составления списка рабочих было прервано.").toStdString());
        }
        // Вставляем студента в конец вектора mStudent
        workers.push_back(worker);
    }
    endResetModel();

    return workers.size();
}

bool WorkerList::haveWorker(const Worker *worker)
{
    for (const auto& element : workers)
    {
        if (element == *worker) return true;
    }

    return false;
}

Worker &WorkerList::worker(const QModelIndex index)
{
    return workers[index.row()];
}

void WorkerList::setMode(bool value)
{
    this->mode = value;
}

size_t WorkerList::size() const
{
    return workers.size();
}
