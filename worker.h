#ifndef WORKER_H
#define WORKER_H

#include "user.h"
#include "person.h"

#include <QString>
#include <QDataStream>
#include <QTextStream>

class Worker : public User, public Person
{
public:
    Worker();
    // Перегружаем оператор присваивания.
    Worker& operator=(const Worker &other)
    {
        this->setUserData(other.login(), other.password(), other.right());
        this->setPersonData(other.firstName(), other.secondName(), other.middleName());
        return *this;
    }

    void save(QDataStream& ost) const;
    void load(QDataStream& ist);

    bool operator==(const Worker& other) const
    {
        return (this->login() == other.login());
    }
};

inline QDataStream &operator<<(QDataStream &ost, const Worker &worker)
{
    worker.save(ost);
    return ost;
}

inline QDataStream &operator>>(QDataStream &ist, Worker &worker)
{
    worker.load(ist);
    return ist;
}

#endif // WORKER_H
