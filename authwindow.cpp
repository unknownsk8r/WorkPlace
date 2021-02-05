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
    if (!findAdmin()) {
        currentUser.setPersonData("admin", "admin", "admin");
        currentUser.setUserData("admin", "admin", User::Right::Admin);
        users.push_back(currentUser);
    }
    qDebug() << users.size();

    try
    {
        QString login = ui->loginEdit->text();
        QString password = ui->passwordEdit->text();

        if (login.isEmpty() || password.isEmpty())
        {
            throw std::invalid_argument("введите логин и/или пароль");
        }

        // Ищем пользователя по логину и паролю.
        if (!findUser(login, password)) {
            throw std::invalid_argument("такого пользователя нет");
        }
    }
    catch (const std::exception &exc)
    {
        QMessageBox::critical(this, Config::applicationName, tr("Ошибка авторизации: ") + exc.what());
        return;
    }

    start();
}

bool AuthWindow::uploadDatabase(const char path[])
{
    try
    {
        QFile inf(path);

        // Открываем файл только для чтения
        if (!inf.open(QIODevice::ReadOnly))
        {
            throw std::runtime_error(inf.errorString().toStdString());
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

bool AuthWindow::findUser(const QString& login, const QString& password)
{
    for (const auto& user : users)
    {
        // Если введенные логин и пароль (далее — ЛиП) совпадают с ЛиП
        // пользователя из базы данных, присваем переменной currentUser
        // его значение.
        if (user.login() == login && user.password() == password)
        {
            currentUser = user;
            return true;
        }
    }

    return false;
}

bool AuthWindow::findAdmin()
{
    for (const auto& user : users)
    {
        // Если введенные логин и пароль (далее — ЛиП) совпадают с ЛиП
        // пользователя из базы данных, присваем переменной currentUser
        // его значение.
        if (user.right() == User::Right::Admin)
        {
            return true;
        }
    }

    return false;
}

void AuthWindow::start()
{
    // Динамически выделяем память под окно управления (чтобы после выхода из функции она не уничтожилась).
    Admin *w = new Admin;
    // Устанавливаем нужный заголовок (в нашем случае это "Workplace")
    w->setWindowTitle("WorkPlace");
    // Устанавливаем пользователя
    w->setUser(&currentUser);
    // Загружаем пользователей и задачи из база данных
    w->loadFromDatabase();
    // Отображаем на экране.
    w->show();
    // Скрываем окно авторизации.
    QDialog::accept();
}
