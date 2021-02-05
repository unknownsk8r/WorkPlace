#include "taskwindow.h"
#include "ui_taskwindow.h"

#include <QMessageBox>
#include <QDebug>
#include <stdexcept> // runtime_error

TaskWindow::TaskWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TaskWindow)
{
    ui->setupUi(this);
    // Устанавливаем время начала выполнения работы как текущее время,
    // т.к. это является самым распространенным (или приближенным) сценарием.
    ui->startTimeEdit->setDateTime(QDateTime::currentDateTime());
    ui->endTimeEdit->setDateTime(QDateTime::currentDateTime());
    // Делаем поля "ID" и "ФИО исполнителя" недоступными для ручного редактирования.
    ui->idEdit->setEnabled(false);
    ui->workerEdit->setEnabled(false);
    // Меняем стандартные модели на пользовательские.
    ui->employerTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

TaskWindow::~TaskWindow()
{
    delete ui;
}

void TaskWindow::setTask(Task *currentTask)
{
    this->task_ptr = currentTask;
}

void TaskWindow::updateTask(Task *updatingTask)
{
    ui->acceptButton->setText("Изменить");
    // Задание ...
    ui->taskEdit->setText(updatingTask->task());
    // Сотрудники ...
    ui->dispatcherEdit->setText(updatingTask->dispatcher().fullName());
    ui->workerEdit->setText(updatingTask->worker().fullName());
    // Время ...
    ui->startTimeEdit->setDateTime(updatingTask->startTime());
    ui->endTimeEdit->setDateTime(updatingTask->endTime());
}

bool TaskWindow::setDispatcher(Worker *dispatcher)
{
    // Проверяем наличие указателя на пользователя.
    if (dispatcher == nullptr)
    {
        throw std::runtime_error(tr("был передан пустой указатель на диспетчера.").toStdString());
        return false;
    }

    if ((dispatcher->right() == User::Right::Dispatcher) || (dispatcher->right() == User::Right::Admin))
    {
        this->dispatcher_ptr = dispatcher;
        // Устанавливаем поля "ФИО диспетчера" равным ФИО пользователя, переданного
        // по указателю.
        ui->dispatcherEdit->setText(this->dispatcher_ptr->fullName());
        // Запрещаем ручное редактирование поля "ФИО диспетчера".
        ui->dispatcherEdit->setEnabled(false);

        return true;
    }

    // Если пользователь не является диспетчером или администратором, кидаем исключительную ситуацию.
    throw std::logic_error(tr("пользоваль не обладает правами диспетчера.").toStdString());
    return false;
}

bool TaskWindow::setWorkerr(Worker *worker)
{
    if (worker == nullptr)
    {
        return false;
    }
    else
    {
        this->employer_ptr = worker;
        return true;
    }
}

void TaskWindow::accept()
{
    try
    {
        // Диспетчеру необходимо указать рабочего, который выполнит задание.
        if (employer_ptr == nullptr) throw std::logic_error("Выберите сотрудника для выполнения задания!");

        if (employer_ptr->right() != User::Right::Worker) throw std::logic_error("Выберите рабочего для выполнения задания");
        // Рабочий и диспетчер не должны быть одним и тем же человеком.
        if (*employer_ptr == *dispatcher_ptr) throw std::logic_error("Выбранный для выполнения задания работник "
                                                              "не может быть диспетчером или администратором.");
        // Диспетчер обязан корректно заполнить все поля в форме.
        isValid();
    }
    catch (const std::exception &exc)
    {
        QMessageBox::warning(
                            this,
                            QString("Предупреждение"),
                            exc.what(),
                            QMessageBox::Ok
                            );
        return;
    }

    task_ptr->setTask(ui->taskEdit->toPlainText());
    task_ptr->setEmployers(*dispatcher_ptr, *employer_ptr);
    task_ptr->setTime(ui->startTimeEdit->dateTime(), ui->endTimeEdit->dateTime());

    workerlist_ptr.release();
    QDialog::accept();
}

void TaskWindow::reject()
{
    workerlist_ptr.release();
    QDialog::reject();
}

void TaskWindow::setWorker(QModelIndex index)
{
    this->employer_ptr = &workerlist_ptr->worker(index);
    ui->workerEdit->setText(this->employer_ptr->fullName());
}

bool TaskWindow::isValid()
{
    if  (ui->taskEdit->toPlainText().isEmpty())
    {
        throw std::logic_error("Заполните поле \"Текст задания\"");
        return false;
    }
    else if (ui->startTimeEdit->dateTime() >= ui->endTimeEdit->dateTime())
    {
        throw std::logic_error("Введено некорректное время выполнения работы: <br>"
                                 "1. Время начала и конца выполнения работы не должны совпадать. <br>"
                                 "2. Время начала выполнения работы должно быть меньше, чем время конца выполнения работы.");
        return false;
    }

    return true;
}

void TaskWindow::setWorkerTable(WorkerList *workerList)
{
    workerlist_ptr.reset(workerList);
    ui->employerTable->setModel(workerlist_ptr.get());
}
