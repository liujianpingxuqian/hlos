#ifndef INCLUDE_TYPES_H_
#define INCLUDE_TYPES_H_

#include <stddef.h>

/*
#define BITS_PER_LONG	32

#define BITS_TO_LONGS(bits) \
	(((bits)+BITS_PER_LONG-1)/BITS_PER_LONG)
#define DECLARE_BITMAP(name,bits) \
	unsigned long name[BITS_TO_LONGS(bits)]
#define CLEAR_BITMAP(name,bits) \
	memset(name, 0, BITS_TO_LONGS(bits)*sizeof(unsigned long))
*/

/* 32bit platform */
typedef unsigned int 		__kernel_size_t;
typedef __kernel_size_t		size_t;
typedef __kernel_size_t		ssize_t;

/* bsd */
typedef unsigned char		u_char;
typedef unsigned short		u_short;
typedef unsigned int		u_int;
typedef unsigned long		u_long;

/* sysv */
typedef unsigned char		unchar;
typedef unsigned short		ushort;
typedef unsigned int		uint;
typedef unsigned long		ulong;

typedef unsigned char		bool;

/* bit types defined */
typedef signed char		s8;
typedef unsigned char		u8;

typedef signed short		s16;
typedef unsigned short		u16;

typedef signed int		s32;
typedef unsigned int		u32;

typedef signed long long	s64;
typedef unsigned long long 	u64;

/* signed and unsigned bits */
typedef __signed__ char		__s8;
typedef unsigned char		__u8;

typedef __signed__ short	__s16;
typedef unsigned short		__u16;

typedef __signed__ int		__s32;
typedef unsigned int		__u32;

typedef __signed__ long long	__s64;
typedef unsigned long long	__u64;

/* BIT_TYPES_DEFINED */
typedef		__u8		u_int8_t;
typedef		__s8		int8_t;
typedef		__u16		u_int16_t;
typedef		__s16		int16_t;
typedef		__u32		u_int32_t;
typedef		__s32		int32_t;

typedef		__u8		uint8_t;
typedef		__u16		uint16_t;
typedef		__u32		uint32_t;

typedef		__u64		uint64_t;
typedef		__u64		u_int64_t;
typedef		__s64		int64_t;
/* BIT_TYPES_DEFINED */

/* this is a special 64bit data type that is 8-byte aligned */
#define aligned_u64 __u64 __attribute__((aligned(8)))
#define aligned_be64 __be64 __attribute__((aligned(8)))
#define aligned_le64 __le64 __attribute__((aligned(8)))

/* task */
typedef uint32_t pid_t;

typedef u32 dma_addr_t;
typedef u32 phys_addr_t;
typedef phys_addr_t resource_size_t;

struct list_head {
	struct list_head *prev, *next;
};

struct hlist_node {
	struct hlist_node *next, **pprev;
};

struct hlist_head {
	struct hlist_node *first;
};

typedef struct {
	int counter;
} atomic_t;

#endif 	// INCLUDE_TYPES_H_
