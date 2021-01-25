#include "authwindow.h"
#include "ui_authwindow.h"
#include "config.h"
#include "admin.h"

#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QSaveFile>
#include <QtGlobal> // qVersion()
#include <QDebug>
#include <stdexcept>

AuthWindow::AuthWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AuthWindow)
{
    ui->setupUi(this);
    // Загружаем базу данных пользователей.
    uploadDatabase(Config::employersDatabaseFileName);
    ui->passwordEdit->setEchoMode(QLineEdit::Password);
}

AuthWindow::~AuthWindow()
{
    delete ui;
}

Worker AuthWindow::user()
{
    return this->currentUser;
}

void AuthWindow::switchEchoMode(int value)
{
    ui->passwordEdit->setEchoMode((value == 2) ? QLineEdit::Normal : QLineEdit::Password);
}

void AuthWindow::login()
{
    if (!users.isEmpty())
    {
        QString login = ui->loginEdit->text();
        QString password = ui->passwordEdit->text();

        if (login.isEmpty() || password.isEmpty())
        {
            // Если при авторизации пользователь не ввел логин и/или пароль, выводим ошибку.
            QMessageBox::warning(this, tr("Ошибка авторизации"), tr("Для входа необходимо ввести логин и пароль"));
            return;
        }

        // Ищем пользователя по логину и паролю.
        findUser(login, password);

        // Если у пользователя нет прав (т.е. пользователя нет в базе данных), прерываем попытку
        // авторизации.
        if (this->currentUser.right() == User::Right::None)
        {
            QMessageBox::critical(this, Config::applicationName, tr("Пользователя с таким логином и/или паролем не найдено"));
            return;
        }
    }



    // Скрываем окно авторизации.
    QDialog::accept();
}

bool AuthWindow::uploadDatabase(const char path[])
{
    try
    {
        QFile inf(path);

        // Открываем файл только для чтения
        if (!inf.open(QIODevice::ReadOnly))
        {
            throw std::runtime_error((tr("open(): ") + inf.errorString()).toStdString());
        }

        // Привязываем к файлу текстовый поток.
        QDataStream ist(&inf);

        while (!ist.atEnd())
        {
            Worker employer;
            // Читаем пользователя из потока.
            ist >> employer;
            // Если возникла ошибка, запускаем исключительную ситуацию
            if (ist.status() == QDataStream::ReadCorruptData)
            {
                throw std::runtime_error(tr("данные для чтения повреждены.").toStdString());
            }

            // Вставляем пользователя в конец вектора aUsers
            users.push_back(employer);
        }
    }
    catch (const std::exception &exp)
    {
        // Если при открытии файла возникла исключительная ситуация, сообщить пользователю
        QMessageBox::critical(this, Config::applicationName, tr("Невозможно открыть файл: %1").arg(exp.what()));
        return false;
    }

    return true;
}

void AuthWindow::findUser(const QString& login, const QString& password)
{
    for (const auto& user : users)
    {
        // Если введенные логин и пароль (далее — ЛиП) совпадают с ЛиП
        // пользователя из базы данных, присваем переменной currentUser
        // его значение.
        if (user.login() == login && user.password() == password)
        {
            this->currentUser = user;
        }
    }
}
