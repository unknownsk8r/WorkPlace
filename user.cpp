#include "user.h"
#include <stdexcept>

QString User::login() const
{
    return u_login;
}

QString User::password() const
{
    return u_password;
}

User::Right User::right() const
{
    return u_right;
}

QString User::rightToString() const
{
    switch (u_right)
    {
    case User::Right::None:
        return "Нет прав";
        break;
    case User::Right::Worker:
        return "Рабочий";
        break;
    case User::Right::Dispatcher:
        return "Диспетчер";
        break;
    case User::Right::Admin:
        return "Администратор";
        break;
    }
}

bool User::setUserData(const QString& login, const QString& password, const User::Right right)
{
    if (login.isEmpty() || password.isEmpty() || right == User::Right::None)
    {
        throw std::invalid_argument("Некорректные данные");
    }

    this->u_login = login;
    this->u_password = password;
    this->u_right = right;

    return true;
}
