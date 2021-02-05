#include <QtTest>
#include <stdexcept>
#include "../workPlace-master/user.h"
#include "../workPlace-master/person.h"
#include "../workPlace-master/worker.h"
#include "../workPlace-master/task.h"

#include "../workPlace-master/tasktable.h"
#include "../workPlace-master/workerlist.h"

// CPP metatypes
Q_DECLARE_METATYPE(User);
Q_DECLARE_METATYPE(Person);
Q_DECLARE_METATYPE(Worker);
Q_DECLARE_METATYPE(Task);

class tworkplace : public QObject
{
    Q_OBJECT

public:
    tworkplace();
    ~tworkplace();


private slots:
    void checkPerson_case_validData();
    void checkPerson_case_invalidData();

    void checkTask_case_invalidTime();
    void checkTask_case_invalidTask();
    void checkTask_case_sameEmployee();

    void checkUser_case_validData();
    void checkUser_case_invalidData();

    void checkTaskWithDeletedEmployee();
};

tworkplace::tworkplace()
{

}

tworkplace::~tworkplace()
{

}

void tworkplace::checkPerson_case_validData()
{
    Person testPerson;
    // Корректные данные
    QCOMPARE(testPerson.setPersonData("Арсен", "Саппык", "Романович"), true);
}

void tworkplace::checkPerson_case_invalidData()
{
    Person testPerson;
    // Корректные данные
    QVERIFY_EXCEPTION_THROWN(testPerson.setPersonData("", "Саппык", "Романович"), std::invalid_argument);
    QVERIFY_EXCEPTION_THROWN(testPerson.setPersonData("Арсен", "", "Романович"), std::invalid_argument);
    QVERIFY_EXCEPTION_THROWN(testPerson.setPersonData("Арсен", "Саппык", ""), std::invalid_argument);
}

void tworkplace::checkTask_case_sameEmployee()
{
    Task testTask;
    Worker testWorker;
    // Пытаемся назначить одного работника как диспетчера и выполняющего работу
    QVERIFY_EXCEPTION_THROWN(testTask.setEmployers(testWorker, testWorker), std::logic_error);
}

void tworkplace::checkUser_case_validData()
{
    User testUser;
    QCOMPARE(testUser.setUserData("login", "password", User::Right::Admin), true);
}

void tworkplace::checkUser_case_invalidData()
{
    User testUser;
    QVERIFY_EXCEPTION_THROWN(testUser.setUserData("", "password", User::Right::Admin), std::invalid_argument);
    QVERIFY_EXCEPTION_THROWN(testUser.setUserData("login", "", User::Right::Admin), std::invalid_argument);
    QVERIFY_EXCEPTION_THROWN(testUser.setUserData("login", "password", User::Right::None), std::invalid_argument);
}

void tworkplace::checkTaskWithDeletedEmployee()
{
    Worker testDispatcher;
    QCOMPARE(testDispatcher.setPersonData("Арсен", "Саппык", "Романович"), true);
    QCOMPARE(testDispatcher.setUserData("sap", "sap", User::Right::Dispatcher), true);

    Worker testWorker;
    QCOMPARE(testWorker.setPersonData("Владислав", "Белоусов", "Евгеньевич"), true);
    QCOMPARE(testWorker.setUserData("bel", "bel", User::Right::Worker), true);

    Task testTask1;
    testTask1.setTask("TEXT1");
    testTask1.setEmployers(testDispatcher, testWorker);
    TaskTable testTaskTable;
    testTaskTable.insert(testTask1);

    QVERIFY(testTaskTable.size() == 1);

    Task testTask2;
    testTask2.setTask("TEXT2");
    testTask2.setEmployers(testDispatcher, testWorker);
    testTaskTable.insert(testTask2);

    QVERIFY(testTaskTable.size() == 2);

    testTaskTable.deleteTaskByWorker(testWorker);

    QVERIFY(testTaskTable.size() == 0);
}

void tworkplace::checkTask_case_invalidTask()
{
    Task testTask;
    // Пытаемся назначить одного работника как диспетчера и выполняющего работу
    QVERIFY_EXCEPTION_THROWN(testTask.setTask(""), std::invalid_argument);
}

void tworkplace::checkTask_case_invalidTime()
{
    Task testTask;
    QVERIFY_EXCEPTION_THROWN(testTask.setTime(QDateTime::currentDateTime().addDays(1), QDateTime::currentDateTime()), std::invalid_argument);
}

QTEST_APPLESS_MAIN(tworkplace)

#include "tst_tworkplace.moc"
