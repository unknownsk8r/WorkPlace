#include "admin.h"
#include "authwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    AuthWindow auth;
    auth.setWindowTitle("Авторизация");
    auth.show();
    return a.exec();
}
