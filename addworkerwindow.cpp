#include "addworkerwindow.h"
#include "ui_addworkerwindow.h"

#include <QDebug>
#include <QMessageBox>

AddWorkerWindow::AddWorkerWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddWorkerWindow)
{
    ui->setupUi(this);
    ui->isWorker->setChecked(true);
}

AddWorkerWindow::~AddWorkerWindow()
{
    delete ui;
}

void AddWorkerWindow::setWorker(Worker *worker)
{
    this->worker_ptr = worker;
}

void AddWorkerWindow::updateWorker(Worker *worker)
{
    ui->addWorker->setText("Изменить");

    ui->firstNameEdit->setText(worker->firstName());
    ui->secondNameEdit->setText(worker->secondName());
    ui->middleNameEdit->setText(worker->middleName());
    ui->loginEdit->setText(worker->login());
    ui->passwordEdit->setText(worker->password());

    switch (worker->right()) {
    case User::Right::Worker:
        ui->isWorker->setChecked(true);
        break;
    case User::Right::Dispatcher:
        ui->isDispatcher->setChecked(true);
        break;
    case User::Right::Admin:
        ui->isAdmin->setChecked(true);
        break;
    case User::Right::None:
        break;
    }
}

void AddWorkerWindow::accept()
{
    if (!isValid()) {
        QMessageBox::warning(
                    this,
                    QString("Предупреждение"),
                    QString("Заполните все поля"),
                    QMessageBox::Ok
                    );
        return;
    }

    User::Right employerRight = ui->isAdmin->isChecked()
            ? User::Right::Admin                // Если выбран "Администратор",ставим роль администратора
            : (ui->isDispatcher->isChecked()    // Если нет, то...
               ? User::Right::Dispatcher        // Если выбран "Диспетчер", ставим роль диспетчера
               : User::Right::Worker);          // Если нет, то ставим роль рабочего

    worker_ptr->setPersonData(ui->firstNameEdit->text(), ui->secondNameEdit->text(), ui->middleNameEdit->text());
    worker_ptr->setUserData(ui->loginEdit->text(), ui->passwordEdit->text(), employerRight);

    QDialog::accept();
}

bool AddWorkerWindow::isValid()
{
    return  !(
            ui->firstNameEdit->text().isEmpty() ||
            ui->secondNameEdit->text().isEmpty() ||
            ui->middleNameEdit->text().isEmpty() ||
            ui->loginEdit->text().isEmpty() ||
            ui->passwordEdit->text().isEmpty()
            );
}
