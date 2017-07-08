/*
 * =====================================================================================
 *
 *       Filename:  string.h
 *
 *    Description:  字符串处理函数
 *
 *        Version:  1.0
 *        Created:  2013年11月06日 20时12分44秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hurley (LiuHuan), liuhuan1992@gmail.com
 *        Company:  Class 1107 of Computer Science and Technology
 *
 * =====================================================================================
 */

#ifndef INCLUDE_STRING_H_
#define INCLUDE_STRING_H_

#include <types.h>

extern void * memcpy(void *,const void *,size_t);
extern void * memset(void *,int,size_t);
extern void * memmove(void *,const void *,size_t);
extern void * memscan(void *,int,size_t);
extern int memcmp(const void *,const void *,size_t);
extern void * memchr(const void *,int,size_t);

extern bool sysfs_streq(const char *s1, const char *s2);
extern int strtobool(const char *s, bool *res);


extern char * strcpy(char *,const char *);
extern char * strncpy(char *,const char *, size_t);
size_t strlcpy(char *, const char *, size_t);
extern char * strcat(char *, const char *);
extern char * strncat(char *, const char *, size_t);
extern size_t strlcat(char *, const char *, size_t);
extern int strcmp(const char *cs, const char *ct);
extern int strncmp(const char *,const char *,size_t);
extern int strnicmp(const char *, const char *, size_t);
extern int strcasecmp(const char *s1, const char *s2);
extern int strncasecmp(const char *s1, const char *s2, size_t n);
extern char * strchr(const char *,int);
extern char * strnchr(const char *, size_t, int);
extern char * strrchr(const char *,int);
extern char * skip_spaces(const char *);
extern char *strim(char *);
extern char * strstr(const char *, const char *);
extern char * strnstr(const char *, const char *, size_t);
extern size_t strlen(const char *);
extern size_t strnlen(const char *,size_t);
extern char * strpbrk(const char *,const char *);
extern char * strsep(char **,const char *);
extern size_t strspn(const char *,const char *);
extern size_t strcspn(const char *,const char *);

static inline char *strstrip(char *str)
{
	return strim(str);
}
static inline void bzero(void *dest, size_t len)
{
	memset(dest, 0, len);
}
/**
 * strstarts - does @str start with @prefix?
 * @str: string to examine
 * @prefix: prefix to look for.
 */
static inline bool strstarts(const char *str, const char *prefix)
{
	return strncmp(str, prefix, strlen(prefix)) == 0;
}

/**
 * kbasename - return the last part of a pathname.
 *
 * @path: path to extract the filename from.
 */
static inline const char *kbasename(const char *path)
{
	const char *tail = strrchr(path, '/');
	return tail ? tail + 1 : path;
}

#endif 	// INCLUDE_STRING_H_

