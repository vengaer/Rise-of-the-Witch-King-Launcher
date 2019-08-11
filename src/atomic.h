#ifndef ATOMIC_H
#define ATOMIC_H

#ifdef __cplusplus
extern "C" {
#endif

int atomic_inc(int* value);
int atomic_dec(int* value);
int atomic_add(int* value, int inc);
int atomic_sub(int* value, int dec);

int atomic_and(int* value, int rhs);
int atomic_or(int* value, int rhs);
int atomic_xor(int* value, int rhs);

int atomic_fetch_add(int* value, int inc);
int atomic_fetch_sub(int* value, int sub);

int atomic_fetch_and(int* value, int rhs);
int atomic_fetch_or(int* value, int rhs);
int atomic_fetch_xor(int* value, int rhs);

int atomic_read(int* value);
int atomic_write(int* value, int new_value);

#ifdef __cplusplus
}
#endif
#endif
