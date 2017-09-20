/*
 * =====================================================================================
 *
 *       Filename:  console.c
 *
 *    Description:  屏幕操作的一些函数实现
 *
 *        Version:  1.0
 *        Created:  2013年11月02日 21时58分00秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hurley (LiuHuan), liuhuan1992@gmail.com
 *        Company:  Class 1107 of Computer Science and Technology
 *
 * =====================================================================================
 */

#include <io.h>
#include <console.h>

/*
 * VGA(Video Graphics Array，视频图形阵列)是使用模拟信号的一种视频传输标准，内核可以通过它来控制屏幕上字符或者图形的显示。
 * 在默认的文本模式(Text-Mode)下，VGA控制器保留了一块内存(0x8b000~0x8bfa0)作为屏幕上字符显示的缓冲区，
 * 若要改变屏幕上字符的显示，只需要修改这块内存就好了。
 *
 */

// 屏幕"光标"的坐标
static uint8_t xpos = 0;
static uint8_t ypos = 0;
static uint8_t max_xpos = 80;
static uint8_t max_ypos = 25;

/* kernel virtual memroy map here */
#define PAGE_OFFSET	(0xC0000000)

// VGA 的显示缓冲的起点是 0xB8000
// add PAGE_OFFSET for var_addr about VGA access
static uint16_t *video_memory = (uint16_t *)(0xB8000 + PAGE_OFFSET);

// 移动光标
static void move_cursor()
{
	// 屏幕是 80 字节宽
	uint16_t cursorLocation = ypos * 80 + xpos;
	
	// VGA 内部的寄存器多达300多个，显然无法一一映射到I/O端口的地址空间。
	// 对此 VGA 控制器的解决方案是，将一个端口作为内部寄存器的索引：0x3D4，
	// 再通过 0x3D5 端口来设置相应寄存器的值。
	// 在这里用到的两个内部寄存器的编号为14与15，分别表示光标位置的高8位与低8位。

	outb(0x3D4, 14);                  	// 告诉 VGA 我们要设置光标的高字节
	outb(0x3D5, cursorLocation >> 8); 	// 发送高 8 位
	outb(0x3D4, 15);                  	// 告诉 VGA 我们要设置光标的低字节
	outb(0x3D5, cursorLocation);     	// 发送低 8 位
}

// 清屏操作
void console_init(void)
{
	int i;

	for (i = 0; i < max_xpos * max_ypos; i++) {
	      video_memory[i] = 0x720;
	}

	xpos = 0;
	ypos = 0;
	move_cursor();
}

void console_vga_write(const char *str, unsigned n)
{
	char c;
	int  i, k, j;

	while ((c = *str++) != '\0' && n-- > 0) {
		if (ypos >= max_ypos) {
			/* scroll 1 line up */
			for (k = 1, j = 0; k < max_ypos; k++, j++) {
				for (i = 0; i < max_xpos; i++) {
					video_memory[max_xpos*j+i] = video_memory[max_xpos*k+i];
				}
			}
			for (i = 0; i < max_xpos; i++)
				video_memory[max_xpos*j+i] = 0x720;
			ypos = max_ypos-1;
		}
		if (c == '\n') {
			xpos = 0;
			ypos++;
		} else if (c != '\r')  {
			video_memory[max_xpos*ypos+xpos] = (0x7 << 8) | (unsigned short) c;
			xpos++;
			if (xpos >= max_xpos) {
				xpos = 0;
				ypos++;
			}
		}
	}

	move_cursor();
}

/* debug printout for arch */
#include <stdargs.h>
#include <printk.h>
#define LOG_LINE_MAX	1024

int printk(const char *fmt, ...)
{
	static char textbuf[LOG_LINE_MAX];
	char *text = textbuf;
	size_t text_len;
	va_list args;

	va_start(args, fmt);
	text_len = vscnprintf(text, sizeof(textbuf), fmt, args);
	va_end(args);

	console_vga_write(text, text_len);

	return text_len;
}

