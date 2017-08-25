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
#include <pmm.h>
#include <debug.h>
#include <printk.h>
#include <console.h>
#include <page.h>


int kern_init()
{
	init_gdt();
	init_idt();

	console_clear();
	init_debug();
	printk("Hello, OS kernel!\n");

	show_kernel_memory_map();
	show_memory_map();


	init_pmm();

	alloc_page(7);
	alloc_page(5);
	show_free_area();

	//init_timer(200);

	// 开启中断
	//asm volatile ("sti");

	while(true) {
		;
	}

	return 0;
}

