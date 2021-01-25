#ifndef ADDWORKERWINDOW_H
#define ADDWORKERWINDOW_H

#include "worker.h"
#include "workerlist.h"
#include <memory>

#include <QDialog>

namespace Ui {
class AddWorkerWindow;
}

class AddWorkerWindow : public QDialog
{
    Q_OBJECT
public:
    explicit AddWorkerWindow(QWidget *parent = nullptr);
    ~AddWorkerWindow();

    void setWorker(Worker* worker);
    void updateWorker(Worker* worker);

public slots:
    //! Обрабатывает подтверждение диалога.
    void accept() Q_DECL_OVERRIDE;

};

#endif // ADDWORKERWINDOW_H
