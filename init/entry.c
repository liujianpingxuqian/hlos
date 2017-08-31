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
#include <task.h>

static inline void cpu_nop(void)
{
	__asm__ volatile ("nop");
}

static int task_B(void *arg)
{
	int n = 1000;
	printk("\nThis is Task_B\n");

	while(n-- > 0) {
		int i = 100000;

		while(i-- > 0)
			cpu_nop();
		printk(".");
	}
	printk("\nThis is Task_B exit\n");

	return 23;
}

static int task_C(void *arg)
{
	printk("\nThis is Task_B\n");

	while(true) {
		int i = 100000;

		while(i-- > 0)
			cpu_nop();
		printk("X");
	}

	return 0;
}

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

	/* set current execute-flow as a task */
	task_init();

	create_task(task_B, NULL);
	create_task(task_C, NULL);
	//alloc_page(7);
	//alloc_page(5);
	//show_free_area();

	init_timer(200);

	// 开启中断
	asm volatile ("sti");

	printk("\nThis is Task_A\n");

	while(true) {
		int i = 100000;

		while(i-- > 0)
			cpu_nop();
		printk("O");
	}

	return 0;
}

