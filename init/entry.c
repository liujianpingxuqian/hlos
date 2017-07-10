/*
 * =====================================================================================
 *
 *       Filename:  entry.c
 *
 *    Description:  hurlex 内核的入口函数
 *
 *        Version:  1.0
 *        Created:  2013年10月31日 13时31分03秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hurley (LiuHuan), liuhuan1992@gmail.com
 *        Company:  Class 1107 of Computer Science and Technology
 *
 * =====================================================================================
 */

#include <gdt.h>
#include <idt.h>
#include <timer.h>
#include <debug.h>
#include <printk.h>
#include <console.h>

int kern_entry()
{
	init_debug();
	init_gdt();
	init_idt();

	console_clear();
	printk("Hello, OS kernel!\n");

	init_timer(200);

	// 开启中断
	asm volatile ("sti");

	return 0;
}

