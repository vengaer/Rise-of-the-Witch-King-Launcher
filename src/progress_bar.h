#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H

#ifdef __cplusplus
extern "C" {
#endif

#define PROGRESS_BAR_INDENT 80
#define PROGRESS_BAR_LENGTH 50
#define PROGRESS_BAR_BUF_LENGTH 64
#define PROGRESS_BAR_INDENT_BUF_LENGTH 128

struct progress_bar {
    unsigned volatile progress;
    char buffer[PROGRESS_BAR_BUF_LENGTH];
};

static inline void progress_bar_set(struct progress_bar* pb, unsigned progress) {
    pb->progress = progress;
}

void progress_bar_init(struct progress_bar* pb);
void progress_bar_display(struct progress_bar* pb, char const* desc);
void progress_bar_finish(struct progress_bar* pb, char const* desc);

#ifdef __cplusplus
}
#endif

#endif
