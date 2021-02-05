#ifndef PERSON_H
#define PERSON_H

#include <QString>

class Person
{
public:
    Person() = default;
    //! Создаём тип "Права пользователя"
    enum class userRight { None, Worker, Admin };

    QString firstName() const;
    QString secondName() const;
    QString middleName() const;
    QString fullName() const;

    bool setPersonData(const QString&firstName, const QString&secondName, const QString&middleName);

protected:
    QString first_name;
    QString second_name;
    QString middle_name;
};

#endif // PERSON_H
