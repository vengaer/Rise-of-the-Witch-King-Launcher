#ifndef GUI_ERROR_DIALOG_H
#define GUI_ERROR_DIALOG_H

#include "ts_queue.h"

class gui_error_dialog {
    public:
        gui_error_dialog();
        ~gui_error_dialog();

        void post(char const* msg);

    private:
        ts_queue msg_queue;
        static int master_thread_idx;

        void init();
        void show(char const* msg);
};

#endif
