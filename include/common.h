/*
 * =====================================================================================
 *
 *       Filename:  common.h
 *
 *    Description:  一些类型的定义
 *
 *        Version:  1.0
 *        Created:  2013年11月01日 10时07分26秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hurley (LiuHuan), liuhuan1992@gmail.com
 *        Company:  Class 1107 of Computer Science and Technology
 *
 * =====================================================================================
 */

#ifndef INCLUDE_COMMON_H_
#define INCLUDE_COMMON_H_

#include <types.h>

// 端口写一个字节
void outb(uint16_t port, uint8_t value);

// 端口读一个字节
u8 inb(uint16_t addr);

// 端口读一个字
u16 inw(uint16_t addr);

#endif // INCLUDE_COMMON_H_
