#ifndef ATOMIC_H
#define ATOMIC_H

#ifdef __cplusplus
extern "C" {
#endif

int atomic_inc(int* value);
int atomic_dec(int* value);
int atomic_inc_with(int* value, int inc);
int atomic_dec_with(int* value, int dec);

int atomic_read(int* value);
int atomic_write(int* value, int new_value);

#ifdef __cplusplus
}
#endif
#endif
