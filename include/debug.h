/*
 * =====================================================================================
 *
 *       Filename:  debug.h
 *
 *    Description:  调试相关的函数
 *
 *        Version:  1.0
 *        Created:  2013年11月06日 15时12分48秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hurley (LiuHuan), liuhuan1992@gmail.com
 *        Company:  Class 1107 of Computer Science and Technology
 *
 * =====================================================================================
 */

#ifndef INCLUDE_DEBUG_H_
#define INCLUDE_DEBUG_H_

#include <console.h>
#include <vargs.h>

#define assert(x, info)                                       	\
	do {                                                	\
		if (!(x)) {                                     \
			panic(info); 		     		\
		}                                               \
	} while (0)

// 编译期间静态检测
#define static_assert(x)                                	\
	switch (x) { case 0: case (x): ; }

#define BUG_ON(condition) do { if (condition) panic("BUG"); } while(0)

// 初始化 Debug 信息
void init_debug();

// 打印当前的函数调用栈信息
void panic(const char *msg);

int snprintf(char *buf, size_t size, const char *fmt, ...);
int sprintf(char *buf, const char *fmt, ...);
int vscnprintf(char *buf, size_t size, const char *fmt, va_list args);
int printk(const char *fmt, ...);

#endif 	// INCLUDE_DEBUG_H_
