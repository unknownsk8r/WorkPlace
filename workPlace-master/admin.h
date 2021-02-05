#ifndef ADMIN_H
#define ADMIN_H

#include <memory>
#include <QMainWindow>

// Интерфесы
#include "tasktable.h"
#include "workerlist.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Admin; }
QT_END_NAMESPACE

class Admin : public QMainWindow
{
    Q_OBJECT

public:
    Admin(QWidget *parent = nullptr);
    ~Admin();
    //! Устанавливает пользователя
    void setUser(Worker* user);
    //! Загружает рабочих и задания из базы данных.
    bool loadFromDatabase();

private slots:
    //! ...
    void newWorkerTable();
    //! ...
    void newTaskTable();
    void saveIndex(QModelIndex index);
    void setTaskDone();

    //! Добавляет рабочего
    bool addWorker();
    //! Добавляет задание
    bool addTask();
    //! Изменяет данные о рабочем
    void editWorker(QModelIndex index);
    //! Изменяет данные о задании
    void editTask(QModelIndex index);
    //! Удаляет рабочего/рабочих из рабочего пространства
    void deleteEmployers();
    //! Удаляет задание(я) из рабочего пространства
    void deleteTasks();

    //! Сохраняет рабочих и задания в базу данных
    void saveToDatabase();

    //! Обновляет страницу (загружает актуальные данные из базы данных)
    void reload();

    void on_about_triggered();

signals:
    //! Сигнализирует о том, что нужно проверить пользователя на наличие в базе данных
    void setWindowByRight();
    //! Сигнализирует о том, что нужно обновить базу данных рабочих
    void updateWD();
    //! Сигнализирует о том, что нужно обновить базу данных заданий
    void updateTD();

private:
    void setButtonVisibility();
    //! ...
    bool refreshWindow();
    //! Обновляет базу данных рабочих
    void saveEmployers();
    //! Обновляет базу данных заданий
    void saveTasks();
    //! Загружает актуальную базу данных рабочих
    bool loadEmployers();
    //! Загружает актуальную базу данных заданий
    bool loadTasks();

    //! ...
    void createWorkerTable();
    //! ...
    void setWorkerTable(WorkerList* workerList);

    //! ...
    void createTaskTable();
    //! ...
    void setTaskTable(TaskTable* workerList);

    //! Указатель на таблицу с работниками.
    std::unique_ptr<WorkerList> aWorkerList;
    //! Указатель на таблицу с заданиями.
    std::unique_ptr<TaskTable> aTaskList;
    //! ...
    QModelIndex selectedRow;
    //! Указатель на пользователя.
    Worker* currentWorker = nullptr;
    Ui::Admin *ui;
};
#endif // ADMIN_H
