
#include <task.h>
#include <string.h>
#include <page.h>

static LIST_HEAD(task_list);
struct task_struct *current;

static int nr_pid = 0;

#define STACK_SIZE	(4096)
extern uint32_t kern_stack_top;

void task_init(void)
{
        current = (struct task_struct *)(kern_stack_top - STACK_SIZE);

        bzero(current, sizeof(struct task_struct));
        current->state = TASK_RUNNABLE;
        current->stack = (void *)kern_stack_top;
        current->pid = nr_pid++;
        list_add(&current->list, &task_list);
}

static void task_exit(void)
{
	int ret;

	/* get return from task exit */
	asm volatile ("mov %%eax, %0" : "=r" (ret));

	current->state = TASK_ZOMBIE;
	/* free pid */

	/* kick out from cpu */
	schedule();
}

struct task_struct *create_task(int (*fun)(void *), void *arg)
{
	struct page *page;
	uint32_t *stack_top;
	struct task_struct *task;

	/* get one page for kernel task stack */
	page = alloc_page();
	if (page == NULL)
		return NULL;

	task = page_address(page);
        bzero(task, sizeof(struct task_struct));

        task->state = TASK_UNINIT;
        task->stack = (void *)((uint32_t)task + STACK_SIZE);
        task->pid = nr_pid++;
        list_add(&task->list, &task_list);

	/* make call status in stack, then return to this fun */
	stack_top = (uint32_t *)task->stack;
	*(--stack_top) = (uint32_t)arg;
	*(--stack_top) = (uint32_t)task_exit;
	*(--stack_top) = (uint32_t)fun;
	*(--stack_top) = (uint32_t)task->stack; /* ebp in stack due to C call */
	task->context.esp = (uint32_t)stack_top;
	task->context.ebp = 0;
	task->context.eflags = 0x200;
	/* set READY, let schedule it */
        task->state = TASK_RUNNABLE;

	return task;
}

static void switch_to(struct context *prev, struct context *next)
{
	asm volatile (	"movl %0, %%eax;"
			"movl %%esp, (%%eax);"
			"movl %%ebp, 4(%%eax);"
			"movl %%ebx, 8(%%eax);"
			"movl %%esi, 12(%%eax);"
			"movl %%edi, 16(%%eax);"
			"pushf;"
			"pop %%ecx;"
			"movl %%ecx, 20(%%eax);"
			"movl %1, %%eax;"
			"movl (%%eax), %%esp;"
			"movl 4(%%eax), %%ebp;"
			"movl 8(%%eax), %%ebx;"
			"movl 12(%%eax), %%esi;"
			"movl 16(%%eax), %%edi;"
			"movl 20(%%eax), %%eax;"
			"pushl %%eax;"
			"popf;"
			: "=m"(prev), "=m"(next));	

	return;
}

static inline bool task_is_runnable(struct task_struct *task)
{
	return task->state == TASK_RUNNABLE || task->state == TASK_RUNNING;
}

void schedule(void)
{
	bool found = false;
	struct task_struct *task, *prev_task;

	list_for_each_entry(task, &task_list, list) {
		/* choose another one could run */
		if (task != current && task_is_runnable(task)) {
			found = true;
			break;
		}
	}

	if (!found)
		return;

	list_del(&task->list);
	prev_task = current;
	current = task;
	list_add_tail(&current->list, &task_list);
	switch_to(&(prev_task->context), &(current->context));
}
