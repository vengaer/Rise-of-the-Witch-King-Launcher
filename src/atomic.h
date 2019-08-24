#ifndef ATOMIC_H
#define ATOMIC_H

#ifdef __cplusplus
extern "C" {
#endif

int atomic_inc(int volatile* value);
int atomic_dec(int volatile* value);
int atomic_add(int volatile* value, int inc);
int atomic_sub(int volatile* value, int dec);

int atomic_and(int volatile* value, int rhs);
int atomic_or(int volatile* value, int rhs);
int atomic_xor(int volatile* value, int rhs);

int atomic_fetch_add(int volatile* value, int inc);
int atomic_fetch_sub(int volatile* value, int sub);

int atomic_fetch_and(int volatile* value, int rhs);
int atomic_fetch_or(int volatile* value, int rhs);
int atomic_fetch_xor(int volatile* value, int rhs);

int atomic_read(int volatile* value);
int atomic_write(int volatile* value, int new_value);

#ifdef __cplusplus
}
#endif
#endif
