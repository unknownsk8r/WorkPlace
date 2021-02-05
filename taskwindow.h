#ifndef TASKWINDOW_H
#define TASKWINDOW_H

#include <QDialog>
#include <memory>

#include "task.h"
#include "worker.h"
#include "workerlist.h"

namespace Ui {
class TaskWindow;
}

class TaskWindow : public QDialog
{
    Q_OBJECT

public:
    explicit TaskWindow(QWidget *parent = nullptr);
    ~TaskWindow();

    void setTask(Task* task);
    void updateTask(Task* task);
    bool setDispatcher(Worker* dispatcher);
    bool setWorkerr(Worker* worker);
    void setWorkerTable(WorkerList* workerList);

public slots:
    //! Обрабатывает подтверждение диалога.
    void accept() Q_DECL_OVERRIDE;
    void reject() Q_DECL_OVERRIDE;
private slots:
    void setWorker(QModelIndex);
private:
    bool isValid();

    Task* task_ptr;
    Worker* employer_ptr = nullptr;
    Worker* dispatcher_ptr = nullptr;

    // Указатель на таблицу с работниками.
    std::unique_ptr<WorkerList> workerlist_ptr;

    Ui::TaskWindow *ui;
};

#endif // TASKWINDOW_H
