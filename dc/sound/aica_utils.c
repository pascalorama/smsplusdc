#include "aica_types.h"

/****************** utils *******************************************/
/* Yeah, these are pretty wasteful, but what else are we going to
   do with the RAM?? =) */
static void *malloc_base = (void*)0x100000;

void *
malloc(int size) 
{
  void *base = malloc_base;
  malloc_base += (size/4)*4+1;
  return base;
}

void free(void *f) { }


void *
memcpy(void *dst0, const void *src0, size_t len) 
{
  char *dst = dst0;
  const char *src = src0;
  
  while (len > 0) {
    *dst++ = *src++;
    len--;
  }
  return dst0;
}


void *
memset(void *dst0, int what, size_t len) 
{
  char *dst = dst0;
  
  while (len > 0) {
    *dst++ = what;
    len--;
  }
  return dst0;
}


