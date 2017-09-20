/*
 * =====================================================================================
 *
 *       Filename:  console.h
 *
 *    Description:  屏幕操作的相关函数的头文件
 *
 *        Version:  1.0
 *        Created:  2013年11月02日 21时55分58秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hurley (LiuHuan), liuhuan1992@gmail.com
 *        Company:  Class 1107 of Computer Science and Technology
 *
 * =====================================================================================
 */

#ifndef INCLUDE_CONSOLE_H_
#define INCLUDE_CONSOLE_H_

#include <types.h>

// 清屏操作
void console_init();
void console_vga_write(const char *str, unsigned n);

/* this should be print.c for arch */
int printk(const char *fmt, ...);

#endif  // INCLUDE_CONSOLE_H_
