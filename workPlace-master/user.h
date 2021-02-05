#ifndef USER_H
#define USER_H

#include <QString>
#include <QMetaEnum>

class User
{
public:
    User() = default;

    //! Создаём тип "Права пользователя"
    enum class Right { None, Worker, Dispatcher, Admin };

    QString login() const;
    QString password() const;
    Right right() const;
    QString rightToString() const;

    bool setUserData(const QString &login, const QString &password, const Right right);

protected:
    QString u_login;
    QString u_password;
    Right u_right;
};

#endif // USER_H
