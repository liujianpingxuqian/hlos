#ifndef _INCLUDE_STDDEF_H
#define _INCLUDE_STDDEF_H


#undef NULL
#define NULL ((void *)0)

enum {
	false	= 0,
	true	= 1
};

#define INT_MAX		((int)(~0U>>1))
#define INT_MIN		(-INT_MAX - 1)

#undef offsetof
#ifdef __compiler_offsetof
#define offsetof(TYPE,MEMBER) __compiler_offsetof(TYPE,MEMBER)
#else
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})

#endif
