#ifndef AUTHWINDOW_H
#define AUTHWINDOW_H

#include <QDialog>
#include <QString>

#include <worker.h>

namespace Ui {
class AuthWindow;
}

class AuthWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AuthWindow(QWidget *parent = nullptr);
    ~AuthWindow();

    Worker user();

private slots:
    void switchEchoMode(int);
    void login();

private:
    bool uploadDatabase(const char[]);
    void findUser(const QString& login, const QString& password);

    Ui::AuthWindow *ui;
    Worker currentUser;
    QVector<Worker> users;
};

#endif // AUTHWINDOW_H
