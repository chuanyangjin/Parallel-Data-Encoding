#ifndef _ENCODE_H_
#define _ENCODE_H_

#include "global.h"

// Encoding chunk with maximum size of 4KB
typedef struct {
    unsigned char* string;
    int size;
    int position;
} chunk;

chunk *encode(chunk* input);

#endif