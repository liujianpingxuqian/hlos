#ifndef INCLUDE_SLOB_H_
#define INCLUDE_SLOB_H_

#include <types.h>

void slob_init(void);
void *kmalloc(uint32_t size);
void kfree(void *addr);

#endif
