/*
 * =====================================================================================
 *
 *       Filename:  elf.c
 *
 *    Description:  ELF 格式解析相关函数
 *
 *        Version:  1.0
 *        Created:  2013年11月06日 12时51分24秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hurley (LiuHuan), liuhuan1992@gmail.com
 *        Company:  Class 1107 of Computer Science and Technology
 *
 * =====================================================================================
 */

#include <pmm.h>
#include <string.h>
#include <multiboot.h>
#include <debug.h>
#include <printk.h>
#include <page.h>

#define ELF32_ST_TYPE(i) ((i)&0xf)

// ELF 格式区段头
typedef
struct elf_section_header_t {
  uint32_t name;
  uint32_t type;
  uint32_t flags;
  uint32_t addr;
  uint32_t offset;
  uint32_t size;
  uint32_t link;
  uint32_t info;
  uint32_t addralign;
  uint32_t entsize;
} __attribute__((packed)) elf_section_header_t;

// ELF 格式符号
typedef
struct elf_symbol_t {
  uint32_t name;
  uint32_t value;
  uint32_t size;
  uint8_t  info;
  uint8_t  other;
  uint16_t shndx;
} __attribute__((packed)) elf_symbol_t;


#define KALLSYMS_NUM_MAX	1024
static unsigned int kallsyms_num;
static uint32_t kallsyms_addr[KALLSYMS_NUM_MAX];
static uint32_t kallsyms_name[KALLSYMS_NUM_MAX];

static bool is_kernel_text(uint32_t addr)
{
	if (addr > (uint32_t)kern_text_start + PAGE_OFFSET && addr < (uint32_t)kern_text_end + PAGE_OFFSET)
		return true;

	return false;
}
// 从 multiboot_t 结构获取ELF信息
static void load_ksym_from_multiboot(multiboot_t *mb)
{
	int i, symtab_size = 0;
	const char *strtab_offset;
	elf_symbol_t *ksymtab;
	elf_section_header_t *sh = (elf_section_header_t *)(mb->addr + PAGE_OFFSET);
	/* offset of section header name strings table */
	uint32_t shstrtab = sh[mb->shndx].addr + PAGE_OFFSET;

	printk("ELE Header: 0x%08X, STRTAB: 0x%08X\n", sh, shstrtab);

	for (i = 0; i < mb->num; i++) {
		const char *name = (const char *)(shstrtab + sh[i].name);
		// 在 GRUB 提供的 multiboot 信息中寻找内核 ELF 格式所提取的字符串表和符号表
		if (strcmp(name, ".strtab") == 0) {
			strtab_offset = (const char *)sh[i].addr + PAGE_OFFSET;
			printk("ELF STRTAB offset 0x%08X\n", strtab_offset);
		}

		if (strcmp(name, ".symtab") == 0) {
			ksymtab = (elf_symbol_t *)(sh[i].addr + PAGE_OFFSET);
			symtab_size = sh[i].size / sizeof(elf_symbol_t);
			printk("ELF KSYMTAB offset 0x%08X, size 0x%X\n", ksymtab, symtab_size);
		}
	}

	kallsyms_num = 0;
	for (i = 0; i < symtab_size; i++) {
		if (is_kernel_text(ksymtab[i].value)) {
			kallsyms_addr[kallsyms_num] = ksymtab[i].value;
			kallsyms_name[kallsyms_num] = (uint32_t)strtab_offset + ksymtab[i].name;
			//printk("%d [0x%08X] %s\n", kallsyms_num, kallsyms_addr[kallsyms_num], (const char *)kallsyms_name[kallsyms_num]);
			kallsyms_num ++;
		}
	}
	printk("ksym_num %d\n", kallsyms_num);
	
	/* sort symbol address */
	for (i = 0; i < kallsyms_num; i++) {
		int k, idx = i;
		uint32_t addr_t = kallsyms_addr[idx]; 

		for (k = i+1; k < kallsyms_num; k++) {
			if (addr_t > kallsyms_addr[k]) {
				idx = k;
				addr_t = kallsyms_addr[k];
			}
		}
		if (i != idx) {
			uint32_t str_st = kallsyms_name[idx];

			kallsyms_addr[idx] = kallsyms_addr[i];
			kallsyms_name[idx] = kallsyms_name[i];
			kallsyms_addr[i] = addr_t;
			kallsyms_name[i] = str_st;
		}
	}
}

static unsigned int get_symbol_pos(uint32_t addr, uint32_t *offset)
{
	uint32_t symbol_start = 0;
	unsigned int low, high, mid;

	/* Do a binary search on the sorted kallsyms_addresses array. */
	low = 0;
	high = kallsyms_num;

	while (high - low > 1) {
		mid = low + (high - low) / 2;
		if (kallsyms_addr[mid] <= addr)
			low = mid;
		else
			high = mid;
	}

	/*
	 * Search for the first aliased symbol. Aliased
	 * symbols are symbols with the same address.
	 */
	while (low && kallsyms_addr[low-1] == kallsyms_addr[low])
		--low;

	symbol_start = kallsyms_addr[low];
	if (offset)
		*offset = addr - symbol_start;

	return low;
}

/* lookup symbol name string by address */
static const char *ksym_lookup_name(uint32_t addr, uint32_t *offset)
{
	unsigned int idx;

	idx = get_symbol_pos(addr, offset);

	return (const char *)kallsyms_name[idx];
}

static void print_stack_trace()
{
	uint32_t *ebp, *eip;

	asm volatile ("mov %%ebp, %0" : "=r" (ebp));
	while (ebp) {
		eip = ebp + 1;
		printk("   [0x%x] %s\n", *eip, ksym_lookup_name(*eip, NULL));
		ebp = (uint32_t*)*ebp;
	}
}

/* Debug API bellow */
void init_debug()
{
	// 从 GRUB 提供的信息中获取到内核符号表和代码地址信息
	load_ksym_from_multiboot(glb_mboot_ptr);
}

void print_cur_status()
{
	static int round = 0;
	uint16_t reg1, reg2, reg3, reg4;

	asm volatile ( 	"mov %%cs, %0;"
			"mov %%ds, %1;"
			"mov %%es, %2;"
			"mov %%ss, %3;"
			: "=m"(reg1), "=m"(reg2), "=m"(reg3), "=m"(reg4));

	// 打印当前的运行级别
	printk("%d: @ring %d\n", round, reg1 & 0x3);
	printk("%d:  cs = %x\n", round, reg1);
	printk("%d:  ds = %x\n", round, reg2);
	printk("%d:  es = %x\n", round, reg3);
	printk("%d:  ss = %x\n", round, reg4);
	++round;
}

void panic(const char *msg)
{
	printk("*** System panic: %s\n", msg);
	print_stack_trace();
	printk("***\n");

	// 致命错误发生后打印栈信息后停止在这里
	while(1);
}

void show_kernel_memory_map(void)
{
        printk("kernel in memory start: 0x%08X\n", kern_start);
	printk("kernel in memory end:   0x%08X\n", kern_end);
        
	printk("\nkernel segment in memory:\n");
	printk("  .init.text    0x%08X ~ 0x%08X \n", kern_init_text_start, kern_init_text_end);
	printk("  .init.data    0x%08X ~ 0x%08X \n", kern_init_data_start, kern_init_data_end);
	printk("  .text         0x%08X ~ 0x%08X \n", kern_text_start, kern_text_end);
	printk("  .data         0x%08X ~ 0x%08X \n", kern_data_start, kern_data_end);
        
	printk("\nkernel in memory used: %d KB = %d Pages\n\n",
                        (kern_end - kern_start) / 1024, (kern_end - kern_start) / 1024 / 4);
}

void show_memory_map(void)
{
        uint32_t mmap_addr = glb_mboot_ptr->mmap_addr;
        uint32_t mmap_length = glb_mboot_ptr->mmap_length;

        printk("Memory map:\n\n");

        mmap_entry_t *mmap = (mmap_entry_t *)mmap_addr;
        for (mmap = (mmap_entry_t *)mmap_addr; (uint32_t)mmap < mmap_addr + mmap_length; mmap++) {
                printk("base_addr = 0x%X%08X, length = 0x%X%08X, type = 0x%X\n",
                        (uint32_t)mmap->base_addr_high, (uint32_t)mmap->base_addr_low,
                        (uint32_t)mmap->length_high, (uint32_t)mmap->length_low,
                        (uint32_t)mmap->type);
        }
        printk("\n");
}

