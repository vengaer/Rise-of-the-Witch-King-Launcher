#ifndef PROGRESS_CALLBACK_H
#define PROGRESS_CALLBACK_H

#ifdef __cplusplus
extern "C" {
#endif

struct progress_callback {
    int volatile total;
    int volatile current;
};

inline void progress_init(struct progress_callback* pc);

inline void progress_increment(struct progress_callback* pc);
inline void progress_set_total(struct progress_callback* pc, int new_total);
inline void progress_set_current(struct progress_callback* pc, int new_current);
inline int progress_get_percentage(struct progress_callback const* pc);

#ifdef __cplusplus
}
#endif

#endif
