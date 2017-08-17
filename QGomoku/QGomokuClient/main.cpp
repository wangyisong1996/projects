//#include "clientmainwindow.h"
#include "logindialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //ClientMainWindow w;
    //w.show();

    LoginDialog *dialog = new LoginDialog();
    dialog->show();

    return a.exec();
}
