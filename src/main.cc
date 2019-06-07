#include "cli_interface.h"
#ifndef GTK_GUI
#include "mainwindow.h"
#include <QApplication>
#else
#include "gtk_gui.h"
#endif

int main(int argc, char *argv[]) {
    if(argc > 2)
        return cli_main(argc, argv);
#ifdef GTK_GUI
    gui_init(&argc, &argv);


#else
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
#endif
}
