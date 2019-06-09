#include "bitop.h"
#include <limits.h>

int trailing_zeros(unsigned value) {
    int count;

    if(value) {
        value = (value ^ (value - 1)) >> 1;
        for(count = 0; value; count++)
            value >>= 1;
    }
    else
        count = CHAR_BIT * sizeof(value);
    
    return count;
}
