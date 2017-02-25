#ifndef _AICA_UTILS_H_
#define _AICA_UTILS_H_

#include "aica_types.h"

void *malloc(int size);
void free(void *f);
void *memcpy(void *dst0, const void *src0, size_t len);
void *memset(void *dst0, int what, size_t len);

#endif
