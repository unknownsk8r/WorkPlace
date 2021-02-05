#include "tasks.h"
#include "ui_tasks.h"

Tasks::Tasks(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Tasks)
{
    ui->setupUi(this);
}

Tasks::~Tasks()
{
    delete ui;
}
