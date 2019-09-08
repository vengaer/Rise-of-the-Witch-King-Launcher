#include "cli.h"
#include "command.h"
#include "mainwindow.h"
#include "strutils.h"
#include <QApplication>
#include <omp.h>

#if defined __CYGWIN__ || defined _WIN32
#include <windows.h>
#endif

void (*display_error)(char const*) = NULL;

int main(int argc, char *argv[]) {
    if(argc > 1) {
        display_error = &cli_error_diag;
        return cli_main(argc, argv);
    }

    display_error = &gui_error_diag;

    show_console(false);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
