#include "person.h"
#include <stdexcept>

QString Person::firstName() const
{
    return first_name;
}

QString Person::secondName() const
{
    return second_name;
}

QString Person::middleName() const
{
    return middle_name;
}

QString Person::fullName() const
{
    return second_name + " " + first_name[0] + "." + middle_name[0] + ".";
}

bool Person::setPersonData(const QString &firstName, const QString &secondName, const QString &middleName)
{
    if (firstName.isEmpty() || secondName.isEmpty() || middleName.isEmpty()) // Если пустые
    {
        throw std::invalid_argument("Пустые поля");
    }

    this->first_name = firstName;
    this->middle_name = middleName;
    this->second_name = secondName;
    return true;
}
