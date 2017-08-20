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

void show_kernel_map(void)
{
	printk("\n");
	printk("Kernel image : 0x%x --- 0x%x\n", kern_start, kern_end);
	printk("Kernel Text  : 0x%x --- 0x%x\n", kern_text_start, kern_text_end);
	printk("Kernel Data  : 0x%x --- 0x%x\n", kern_data_start, kern_data_end);
	printk("Kernel Size is %d KB\n", (kern_end - kern_start)>>10);
	printk("\n");
}

int kern_init()
{
	//init_debug();
	//init_gdt();
	//init_idt();

	console_clear();
	printk("Hello, OS kernel!\n");

	show_kernel_map();

	while(true) {
		;
	}

	//init_pmm();

	//init_timer(200);

	// 开启中断
	//asm volatile ("sti");

	return 0;
}

