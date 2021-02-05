#include "admin.h"
#include "ui_admin.h"

// Отдельные С++ классы
#include "worker.h"
#include "task.h"

#include "config.h"

// Всплывающие окна
#include "addworkerwindow.h"
#include "taskwindow.h"

#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QSaveFile>
#include <QtGlobal> // qVersion()
#include <QDebug>
#include <stdexcept>
#include <set>
#include <memory>

Admin::Admin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Admin)
{
    //
    ui->setupUi(this);
    // Настраиваем таблицы, чтобы они длина колонок была одинаковой.
    ui->WorkerList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->TaskList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // Меняем стандартные модели на пользовательские.
    newWorkerTable();
    newTaskTable();

    connect(this, &Admin::updateWD, [this] { saveEmployers(); });
    connect(this, &Admin::updateTD, [this] { saveTasks(); });
    connect(this, &Admin::setWindowByRight, [this] { refreshWindow(); });
}

Admin::~Admin()
{
    delete ui;
}

void Admin::setUser(Worker *user)
{
    this->currentWorker = user;
    emit setWindowByRight();
}

bool Admin::addTask()
{
    if (!(currentWorker->right() == User::Right::Dispatcher))
    {
        QMessageBox::warning(this,
                             tr("Ошибка доступа"),
                             tr("Добавлять задания может только диспетчер"));
        return false;
    }

    // Создаём объект класса "Задание"
    Task currentTask;
    // Создаём окно "Добавление/редактирование задания"
    TaskWindow taskWindow;
    // Устанавливаем заголовок окна
    taskWindow.setWindowTitle("Добавление нового задания");
    // Связываем заметку
    taskWindow.setTask(&currentTask);
    // Пытаемся передать указатель на диспетчера
    try { taskWindow.setDispatcher(currentWorker); }
    catch (const std::exception &e)
    {
        // Если при открытии файла возникла исключительная ситуация, сообщить пользователю
        QMessageBox::critical(this,
                              tr("Критическая ошибка"),
                              tr("Невозможно установить диспетчера: %1").arg(e.what()));
        return false;
    }

    // Передаём в форму указать на таблицу сотрудников
    taskWindow.setWorkerTable(aWorkerList.get());
    // Отображаем окно
    taskWindow.show();
    // Если окно завершилось с сигналом "Прервано", прерываем операцию
    if (taskWindow.exec() != TaskWindow::Accepted)
    {
        return false;
    }
    // Вставляем задачу в таблицу
    aTaskList->insert(currentTask);
    // Обновляем базу данных
    emit updateTD();

    return true;
}

void Admin::editTask(QModelIndex index)
{
    if (!(currentWorker->right() == User::Right::Dispatcher))
    {
        QMessageBox::warning(this,
                             tr("Ошибка доступа"),
                             tr("Редактировать задания может только диспетчер"));
        return;
    }

    Task editingTask = aTaskList->task(index);

    if (editingTask.status() == Task::Status::Done)
    {
        QMessageBox::warning(this,
                             tr("Ошибка доступа"),
                             tr("Нельзя редактировать выполненные задания"));
        return;
    }

    Worker editingWorker = editingTask.worker();
    TaskWindow taskWindow;
    taskWindow.setWindowTitle(tr("Окно редактирования задания"));
    taskWindow.setTask(&editingTask);
    taskWindow.updateTask(&editingTask);
    // Пытаемся передать указатель на диспетчера
    taskWindow.setWorkerr(&editingWorker);
    try { taskWindow.setDispatcher(currentWorker); }
    catch (const std::exception &e)
    {
        // Если при открытии файла возникла исключительная ситуация, сообщить пользователю
        QMessageBox::critical(this,
                              tr("Критическая ошибка"),
                              tr("Невозможно установить диспетчера: %1").arg(e.what()));
        return;
    }
    // Передаём в форму указать на таблицу сотрудников
    taskWindow.setWorkerTable(aWorkerList.get());
    taskWindow.show();

    if (taskWindow.exec() != QDialog::Accepted)
    {
        return;
    }

    aTaskList->edit(index, editingTask);

    emit updateTD();
}

void Admin::deleteTasks()
{
    if (!(currentWorker->right() == User::Right::Dispatcher))
    {
        QMessageBox::warning(this,
                             tr("Ошибка доступа"),
                             tr("Удалять задания из базы данных может только диспетчер"));
        return;
    }

    // Если записная книжка пустая, то удаление заметок невозможно
    if (!(ui->TaskList->model()->rowCount() > 0))
    {
       QMessageBox::warning(this, Config::applicationName, tr("В таблице нет ни одного задания."));
       return;
    }
    // ...
    if (ui->TaskList->selectionModel()->selectedRows().isEmpty())
    {
        QMessageBox::warning(this, Config::applicationName, tr("Выберите задания, которые требуется удалить"));
        return;
    }

    // Передаем в переменную deleteButtonValue значение нажатой кнопки (YES || NO)
    auto deleteButtonValue = QMessageBox::question(this,
                                          QString("Подтверждение"),
                                          QString("Вы действительно хотите удалить эту/эти задания?"),
                                          QMessageBox::Yes | QMessageBox::No);

    if (deleteButtonValue == QMessageBox::Yes) // Нажата кнопка Yes
    {
        // Для хранения номеров строк создаём STL-контейнер "множество", элементы
        // которого автоматически упорядочиваются по возрастанию
        std::set<int> rows;
        {
            // Получаем от таблицы заметок список индексов выбранных в настоящий момент
            // элементов
            QModelIndexList idc = ui->TaskList->selectionModel()->selectedRows();

            // Вставляем номера выбранных строк в rows
            for (const auto &i : idc)
            {
                rows.insert(i.row());
            }
        }
        // Обходим множество номеров выбранных строк *по убыванию*, чтобы удаление предыдущих
        // не сбивало нумерацию следующих
        for (auto it = rows.rbegin(); it != rows.rend(); ++it)
        {
            // Удаляем строку
            aTaskList->erase(*it);
        }

        emit updateTD();
    }
}

bool Admin::addWorker()
{
    if (currentWorker->right() != User::Right::Admin)
    {
        QMessageBox::warning(this,
                             tr("Ошибка доступа"),
                             tr("Добавлять пользователей может только администратор"));
        return false;
    }

    // Создаём объект класса "Работник"
    Worker newWorker;
    // Создаём окно "Добавление/редактирование сотрудника"
    AddWorkerWindow employerWindow;
    employerWindow.setWindowTitle("Добавить нового сотрудника");
    employerWindow.setWorker(&newWorker);
    employerWindow.show();

    if (employerWindow.exec() != AddWorkerWindow::Accepted)
    {
        return false;
    }

    // Вставляем в таблицу.
    try {
        aWorkerList->insert(newWorker);
    }
    catch (const std::exception &e)
    {
        // Если при сохранении рабочего в базу данных возникла исключительная ситуация, сообщить пользователю.
        QMessageBox::critical(this,
                              tr("Критическая ошибка"),
                              tr("%1").arg(e.what()));
        return false;
    }

    emit updateWD();
    return true;
}

void Admin::editWorker(QModelIndex index)
{
    if (currentWorker->right() != User::Right::Admin)
    {
        QMessageBox::warning(this,
                             tr("Ошибка доступа"),
                             tr("Редактировать данные пользователей может только администратор"));
        return;
    }

    Worker worker = aWorkerList->worker(index);
    AddWorkerWindow workerWindow;
    workerWindow.setWindowTitle(tr("Окно редактирования данных сотрудника"));
    workerWindow.setWorker(&worker);
    workerWindow.updateWorker(&worker);
    workerWindow.show();

    if (workerWindow.exec() != QDialog::Accepted)
    {
        return;
    }

    aWorkerList->edit(index, worker);

    emit updateWD();
}

void Admin::deleteEmployers()
{
    if (currentWorker->right() != User::Right::Admin)
    {
        QMessageBox::warning(this,
                             tr("Ошибка доступа"),
                             tr("Удалять пользователей из базы данных может только администратор"));
        return;
    }

    // Если записная книжка пустая, то удаление заметок невозможно
    if (ui->WorkerList->model()->rowCount() == 0)
    {
       QMessageBox::warning(this, Config::applicationName, tr("В таблице нет ни одного рабочего."));
       return;
    }

    // Передаем в переменную deleteButtonValue значение нажатой кнопки (YES || NO)
    auto deleteButtonValue = QMessageBox::question(this,
                                          QString("Подтверждение"),
                                          QString("Вы действительно хотите удалить рабочего (или рабочих)?"),
                                          QMessageBox::Yes | QMessageBox::No);

    if (deleteButtonValue == QMessageBox::Yes) // Нажата кнопка Yes
    {
        // Для хранения номеров строк создаём STL-контейнер "множество", элементы
        // которого автоматически упорядочиваются по возрастанию
        std::set<int> rows;
        {
            // Получаем от таблицы заметок список индексов выбранных в настоящий момент
            // элементов
            QModelIndexList idc = ui->WorkerList->selectionModel()->selectedRows();
            // Вставляем номера выбранных строк в rows
            for (const auto &i : idc)
            {
                // Удаляем строку
                aTaskList->deleteTaskByWorker(aWorkerList->worker(i));
                rows.insert(i.row());
            }
        }

        // Обходим множество номеров выбранных строк *по убыванию*, чтобы удаление предыдущих
        // не сбивало нумерацию следующих
        for (auto it = rows.rbegin(); it != rows.rend(); ++it)
        {          
            aWorkerList->erase(*it);
        }

        emit updateWD();
        emit updateTD();
    }

}

void Admin::saveToDatabase()
{
    saveEmployers();
    saveTasks();
}

bool Admin::loadFromDatabase()
{
    return (loadEmployers() && loadTasks());
}

void Admin::reload()
{
    loadEmployers();
    loadTasks();
}

void Admin::setButtonVisibility()
{

}

bool Admin::refreshWindow()
{
    // Настраиваем общий вид отображения окна
    if (currentWorker->right() == User::Right::Dispatcher || currentWorker->right() == User::Right::Worker)
    {
        ui->addEmployerAction->setVisible(false);
        ui->workerListTitle->setVisible(false);
        ui->WorkerList->setVisible(false);
        ui->groupBox->setVisible(false);
    }

    // Настраиваем кнопки
    ui->setDoneButton->setVisible((currentWorker->right() == User::Right::Worker));
    ui->addTaskButton->setVisible((currentWorker->right() == User::Right::Dispatcher || currentWorker->right() == User::Right::Admin));
    ui->deleteTaskButton->setVisible((currentWorker->right() == User::Right::Dispatcher || currentWorker->right() == User::Right::Admin));

    return true;
}

void Admin::saveEmployers()
{
    // Блок обработки исключительных ситуаций
    try
    {
        QSaveFile outf(Config::employersDatabaseFileName);
        // Открываем файл только для записи
        outf.open(QIODevice::WriteOnly);
        // Привязываем к файлу поток, позволяющий выводить объекты Qt
        QDataStream ost(&outf);
        // Выводим данные в файл
        ost << *aWorkerList;
        // В случае неудачи запускаем исключительную ситуацию (блок прерывается,
        // управление передаётся в блок catch)
        if (!outf.commit())
        {
            throw std::runtime_error(tr("Невозможно закомитить сохранение").toStdString());
        }
    }
    catch (const std::exception &e)
    {
        // Если при сохранении файла возникла исключительная ситуация, сообщить пользователю
        QMessageBox::critical(this, Config::applicationName, tr("Невозможно записать файл %1: %2").arg(Config::employersDatabaseFileName).arg(e.what()));
    }
}

void Admin::saveTasks()
{
    // Блок обработки исключительных ситуаций
    try
    {
        QSaveFile outf(Config::tasksDatabaseFileName);
        // Открываем файл только для записи
        outf.open(QIODevice::WriteOnly);
        // Привязываем к файлу поток, позволяющий выводить объекты Qt
        QDataStream ost(&outf);
        // Выводим данные в файл
        ost << *aTaskList;
        // В случае неудачи запускаем исключительную ситуацию (блок прерывается,
        // управление передаётся в блок catch)
        if (!outf.commit())
        {
            throw std::runtime_error(tr("Невозможно закомитить сохранение").toStdString());
        }
    }
    catch (const std::exception &e)
    {
        // Если при сохранении файла возникла исключительная ситуация, сообщить пользователю
        QMessageBox::critical(this, Config::applicationName, tr("Невозможно записать файл %1: %2").arg(Config::tasksDatabaseFileName).arg(e.what()));
    }
}

bool Admin::loadEmployers()
{
    try
    {
        QFile inf(Config::employersDatabaseFileName);

        // Открываем файл только для чтения
        if (!inf.open(QIODevice::ReadOnly))
        {
            throw std::runtime_error((tr("невозможно открыть файл %1 для чтения: ").arg(Config::employersDatabasePath) + inf.errorString()).toStdString());
        }

        // Привязываем к файлу поток, позволяющий считывать объекты Qt
        QDataStream ist(&inf);

        std::unique_ptr<WorkerList> nb(new WorkerList);
        // Загружаем таблицу из файла
        ist >> *nb;

        // Метод release() забирает указатель у объекта nb
        setWorkerTable(nb.release());
    }
    catch (const std::exception &exp)
    {
        // Если при открытии файла возникла исключительная ситуация, сообщить пользователю
        QMessageBox::critical(this, Config::applicationName, tr("Ошибка: %1").arg(exp.what()));
        return false;
    }

    return true;
}

bool Admin::loadTasks()
{
    try
    {
        QFile inf(Config::tasksDatabaseFileName);

        // Открываем файл только для чтения
        if (!inf.open(QIODevice::ReadOnly))
        {
            throw std::runtime_error((tr("невозможно открыть файл %1 для чтения: ").arg(Config::tasksDatabasePath) + inf.errorString()).toStdString());
        }

        // Привязываем к файлу поток, позволяющий считывать объекты Qt
        QDataStream ist(&inf);

        std::unique_ptr<TaskTable> taskTablePtr(new TaskTable);
        taskTablePtr->setWorker(currentWorker);

        // Загружаем таблицу из файла
        ist >> *taskTablePtr;
        // Метод release() забирает указатель у объекта taskTablePtr
        setTaskTable(taskTablePtr.release());
    }
    catch (const std::exception &exp)
    {
        // Если при открытии файла возникла исключительная ситуация, сообщить пользователю
        QMessageBox::critical(this, Config::applicationName, tr("Ошибка: %1").arg(exp.what()));
        return false;
    }

    return true;
}

void Admin::newWorkerTable()
{
    createWorkerTable();
}

void Admin::newTaskTable()
{
    createTaskTable();
}

void Admin::saveIndex(QModelIndex index)
{
    this->selectedRow = index;
    qDebug() << "INDEX has been saved";
}

void Admin::setTaskDone()
{
    Task task = aTaskList->task(selectedRow);
    qDebug() << task.statusToString();
    task.setStatus(Task::Status::Done);
    qDebug() << task.statusToString();

    aTaskList->edit(selectedRow, task);

    emit updateTD();
}

void Admin::createWorkerTable()
{
    setWorkerTable(new WorkerList);
}

void Admin::setWorkerTable(WorkerList *workerList)
{
    aWorkerList.reset(workerList);
    ui->WorkerList->setModel(aWorkerList.get());
}

void Admin::createTaskTable()
{
    setTaskTable(new TaskTable);
}

void Admin::setTaskTable(TaskTable *tastTable)
{
    aTaskList.reset(tastTable);
    ui->TaskList->setModel(aTaskList.get());
}
