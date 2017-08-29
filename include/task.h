
#ifndef INCLUDE_TASK_H_
#define INCLUDE_TASK_H_

#include <types.h>
#include <list.h>

typedef enum {
	TASK_UNINIT = 0,
	TASK_RUNNABLE,
	TASK_SLEEPING,
	TASK_RUNNING,
	TASK_ZOMBIE,
} task_state;

struct context {
	uint32_t esp;
	uint32_t ebp;
	uint32_t ebx;
	uint32_t esi;
	uint32_t edi;
	uint32_t eflags;
};

struct task_struct {
	task_state state;
	pid_t pid;
	void *stack; /* kernel stack */
	/* struct mm_struct *mm */
	struct context context;
	struct list_head list;
};

void task_init();
void schedule();
struct task_struct *create_task(int (*func)(void *), void *arg);

#endif
