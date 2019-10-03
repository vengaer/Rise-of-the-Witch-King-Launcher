#ifndef GUI_ERROR_DIALOG_H
#define GUI_ERROR_DIALOG_H

#include "ts_queue.h"

class gui_error_dialog {
    public:
        #ifdef _OPENMP
        gui_error_dialog();
        ~gui_error_dialog();
        #endif

        void post(char const* msg);
        void flush();

    private:
        #ifdef _OPENMP
        ts_queue msg_queue;
        static int master_thread_idx;
        #endif

        #ifdef _OPENMP
        void init();
        #endif

        void show(char const* msg);
};

#endif
