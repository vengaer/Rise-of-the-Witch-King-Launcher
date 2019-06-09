#include "cli_interface.h"
#include "command.h"
#include "mainwindow.h"
#include <QApplication>
#include <omp.h>

#if defined __CYGWIN__ || defined _WIN32
#include <windows.h>
#endif
#include "bitop.h"

int main(int argc, char *argv[]) {
    show_console(false);
    if(argc > 1)
        return cli_main(argc, argv);
    
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
