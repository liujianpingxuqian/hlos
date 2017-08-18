#ifndef INCLUDE_PRINTK_H_
#define INCLUDE_PRINTK_H_

#include <types.h>
#include <vargs.h>

int snprintf(char *buf, size_t size, const char *fmt, ...);
int sprintf(char *buf, const char *fmt, ...);
int vscnprintf(char *buf, size_t size, const char *fmt, va_list args);
int printk(const char *fmt, ...);

#endif
