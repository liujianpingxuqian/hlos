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

#include <common.h>
#include <string.h>
#include <elf.h>

#define KALLSYMS_NUM_MAX	1024
static unsigned int kallsyms_num;
static uint32_t kallsyms_addr[KALLSYMS_NUM_MAX];
static uint32_t kallsyms_name[KALLSYMS_NUM_MAX];

extern uint8_t kern_text_start[];
extern uint8_t kern_text_end[];

bool is_kernel_text(uint32_t addr)
{
	if (addr > kern_text_start && addr < kern_text_end)
		return true;

	return false;
}
// 从 multiboot_t 结构获取ELF信息
void load_ksym_from_multiboot(multiboot_t *mb)
{
	int i, symtab_size;
	const char *strtab_offset;
	elf_symbol_t *ksymtab;
	elf_section_header_t *sh = (elf_section_header_t *)mb->addr;
	/* offset of section header name strings table */
	uint32_t shstrtab = sh[mb->shndx].addr;

	for (i = 0; i < mb->num; i++) {
		const char *name = (const char *)(shstrtab + sh[i].name);
		// 在 GRUB 提供的 multiboot 信息中寻找内核 ELF 格式所提取的字符串表和符号表
		if (strcmp(name, ".strtab") == 0)
			strtab_offset = (const char *)sh[i].addr;

		if (strcmp(name, ".symtab") == 0) {
			ksymtab = (elf_symbol_t *)sh[i].addr;
			symtab_size = sh[i].size / sizeof(elf_symbol_t);
		}
	}

	kallsyms_num = 0;
	for (i = 0; i < symtab_size; i++) {
		if (is_kernel_text(ksymtab[i].value)) {
			kallsyms_addr[kallsyms_num] = ksymtab[i].value;
			kallsyms_name[kallsyms_num] = (uint32_t)strtab_offset + ksymtab[i].name;
			kallsyms_num ++;
			//printk("[0x%08x] %s\n", kallsyms_addr[i], (const char *)kallsyms_name[i]);
		}
	}
	
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
	unsigned int i, low, high, mid;

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
const char *ksym_lookup_name(uint32_t addr, uint32_t *offset)
{
	unsigned int idx;

	idx = get_symbol_pos(addr, offset);

	return (const char *)kallsyms_name[idx];
}

