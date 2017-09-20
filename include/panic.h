#ifndef INCLUDE_PANIC_H_
#define INCLUDE_PANIC_H_

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

// 打印当前的函数调用栈信息
void panic(const char *msg);
void print_cur_status();

#endif 	// INCLUDE_PANIC_H_
