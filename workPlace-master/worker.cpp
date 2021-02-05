#include "worker.h"
#include <QDebug>

Worker::Worker()
{
    this->u_right = User::Right::None;
}

void Worker::save(QDataStream &ost) const
{
    ost << this->u_login << this->u_password << this->first_name << this->second_name << this->middle_name;

    switch (this->u_right) {
    case (User::Right::Worker):
        ost << 1;
        break;
    case (User::Right::Dispatcher):
        ost << 2;
        break;
    case (User::Right::Admin):
        ost << 3;
        break;
    default:
        ost << 0;
        break;
    }
}

void Worker::load(QDataStream &ist)
{
    int userRight;
    ist >> this->u_login >> this->u_password >> this->first_name >> this->second_name >> this->middle_name >> userRight;

    if (userRight == 1)           { this->u_right = User::Right::Worker; }
    else if (userRight == 2)      { this->u_right = User::Right::Dispatcher; }
    else if (userRight == 3)      { this->u_right = User::Right::Admin; }
    else                          { this->u_right = User::Right::None; }
}
