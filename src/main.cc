#include "cli_interface.h"
#include "mainwindow.h"
#include <QApplication>

#if defined __CYGWIN__ || defined _WIN32
#include <windows.h>
#endif

int main(int argc, char *argv[]) {
    #if defined __CYGWIN__ || defined _WIN32
        ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
    #endif
    
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
