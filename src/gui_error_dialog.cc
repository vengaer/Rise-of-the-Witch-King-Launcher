#include "gui_error_dialog.h"
#include <QMessageBox>

#ifdef _OPENMP
#include <omp.h>
#endif

gui_error_dialog::gui_error_dialog() {
    init();
}

gui_error_dialog::~gui_error_dialog() {
    #ifdef _OPENMP
    queue_destroy(&msg_queue);
    #endif
}

void gui_error_dialog::post(char const* msg) {
    #ifdef _OPENMP
    if(omp_get_thread_num() == master_thread_idx) {
        char const* m;
        /* Display any queued up messages */
        while(!queue_empty(&msg_queue)) {
            m = (char const*)queue_pop(&msg_queue);
            if(m) {
                show(m);
            }
        }
        show(msg);
    }
    else {
        queue_push(&msg_queue, (void*)msg);
    }
    #else
    show(msg);
    #endif
}

void gui_error_dialog::init() {
    #ifdef _OPENMP
    queue_init(&msg_queue);

    if(master_thread_idx == -1) {
        #pragma omp parallel
        {
            #pragma omp master
            master_thread_idx = omp_get_thread_num();
        }
    }
    #endif
}

void gui_error_dialog::show(char const* msg) {
    QMessageBox box;
    box.critical(0, "Error", msg);
    box.setFixedSize(500, 200);
}

int gui_error_dialog::master_thread_idx{-1};
