/*
 * =====================================================================================
 *
 *       Filename:  types.h
 *
 *    Description:  一些类型的定义
 *
 *        Version:  1.0
 *        Created:  2013年10月31日 16时28分07秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hurley (LiuHuan), liuhuan1992@gmail.com
 *        Company:  Class 1107 of Computer Science and Technology
 *
 * =====================================================================================
 */

#ifndef INCLUDE_TYPES_H_
#define INCLUDE_TYPES_H_

#include <stddef.h>

/* 32bit platform */
typedef unsigned int 	size_t;

/* short type defined */
typedef signed char s8;
typedef unsigned char u8;

typedef signed short s16;
typedef unsigned short u16;

typedef signed int s32;
typedef unsigned int u32;

/* signed and unsigned bits */
typedef __signed__ char __s8;
typedef unsigned char __u8;

typedef __signed__ short __s16;
typedef unsigned short __u16;

typedef __signed__ int __s32;
typedef unsigned int __u32;

typedef		__u8		u_int8_t;
typedef		__s8		int8_t;
typedef		__u16		u_int16_t;
typedef		__s16		int16_t;
typedef		__u32		u_int32_t;
typedef		__s32		int32_t;

typedef		__u8		uint8_t;
typedef		__u16		uint16_t;
typedef		__u32		uint32_t;
#endif 	// INCLUDE_TYPES_H_
