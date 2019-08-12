#include "cli.h"
#include "command.h"
#include "mainwindow.h"
#include <QApplication>
#include <omp.h>

#include "config.h"

#if defined __CYGWIN__ || defined _WIN32
#include <windows.h>
#endif

int main(int argc, char *argv[]) {
    if(argc > 1)
        return cli_main(argc, argv);

    show_console(false);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
