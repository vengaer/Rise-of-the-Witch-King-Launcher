#include "progress_bar.h"
#include <stdio.h>
#include <string.h>

void update_progress_buffer(struct progress_bar* pb);
void format_indent(char* restrict indent, char const* restrict desc);

void progress_bar_init(struct progress_bar* pb) {
    int i;
    for(i = 0; i < PROGRESS_BAR_LENGTH; i++)
        pb->buffer[i] = '-';

    pb->buffer[i] = '\0';
}

void progress_bar_display(struct progress_bar* pb, char const* desc) {
    update_progress_buffer(pb);
    char indent[PROGRESS_BAR_INDENT_BUF_LENGTH];
    format_indent(indent, desc);

    if(desc)
        printf("\r%s%s[%s] : %u%%", desc, indent, pb->buffer, pb->progress);
    else
        printf("\r%s[%s] : %u%%", indent, pb->buffer, pb->progress);
    fflush(stdout);
}

void progress_bar_finish(struct progress_bar* pb, char const* desc) {
    pb->progress = 100;

    update_progress_buffer(pb);

    char indent[PROGRESS_BAR_INDENT_BUF_LENGTH];
    format_indent(indent, desc);

    if(desc)
        printf("\r%s%s[%s] : %u%%", desc, indent, pb->buffer, pb->progress);
    else
        printf("\r%s[%s] : %u%%", indent, pb->buffer, pb->progress);
    fflush(stdout);
}

void update_progress_buffer(struct progress_bar* pb) {
    int i;
    int const nhashes = (double)pb->progress / 100.0 * PROGRESS_BAR_LENGTH;

    for(i = nhashes - 1; i >= 0; --i) {
        if(pb->buffer[i] == '#')
            break;

        pb->buffer[i] = '#';
    }
}

void format_indent(char* restrict indent, char const* restrict desc) {
    unsigned i;
    unsigned desc_len = desc ? strlen(desc) : 0;

    for(i = 0; i < PROGRESS_BAR_INDENT - desc_len; i++)
        indent[i] = ' ';
    indent[i] = '\0';
}
