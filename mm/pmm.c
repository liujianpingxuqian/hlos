#include <multiboot.h>
#include <debug.h>
#include <printk.h>
#include <pmm.h>
#include <string.h>


/* page frame arrary start at kernel end */
static struct page *phy_pages;

/* avriable phy memory */
static uint32_t max_phy_addr;
static uint32_t phy_page_count;
static uint32_t phy_page_start;

static void show_pmm_map()
{
	mmap_entry_t *mmap;
	multiboot_t *mboot = glb_mboot_ptr;
	uint32_t mmap_len, mmap_addr;

	// 打印 GRUB 提供的 由 BIOS 的反馈内存布局
	printk("mem_lower: 0x%X\n", mboot->mem_lower * 1024);
	printk("mem_upper: 0x%X\n\n", mboot->mem_upper * 1024);

	mmap_len = (uint32_t)mboot->mmap_length;
	mmap_addr = (uint32_t)mboot->mmap_addr;

	/* addr_high and len_high is 0 due to 32MB memory limited */
	for (mmap = (mmap_entry_t *) mboot->mmap_addr;(uint32_t)mmap < mmap_addr + mmap_len; mmap++ ) {
		printk("base: length: type          0x%X : 0x%X : 0x%X\n",
			(uint32_t)mmap->base_addr_low, (uint32_t)mmap->length_low, (uint32_t)mmap->type);
	}

	printk("\n");
}

void init_pmm()
{
	mmap_entry_t *mmap;
	uint32_t mmap_len = (uint32_t)glb_mboot_ptr->mmap_length;
	uint32_t mmap_addr = (uint32_t)glb_mboot_ptr->mmap_addr;

	show_pmm_map();

	for (mmap = (mmap_entry_t *) mmap_addr; (uint32_t)mmap < mmap_addr + mmap_len; mmap++ ) {
		/* manage avriable memory above 1MB, and one blank */
		if ((mmap->type == 1) && (mmap->base_addr_low >= 0x100000)) {
			max_phy_addr = mmap->base_addr_low + mmap->length_low;
			phy_page_count = mmap->length_low / PAGE_SIZE;
		}
	}

	phy_pages = (struct page *)(((uint32_t)kern_end + PAGE_SIZE) & PAGE_MASK);
	bzero(phy_pages, phy_page_count * sizeof(struct page));

	phy_page_start = ((uint32_t)phy_pages + phy_page_count * sizeof(struct page) + PAGE_SIZE) & PAGE_MASK;

	printk("Page Phy start 0x%X\n", phy_page_start);
	printk("Page frame start 0x%X, total count 0x%X\n", phy_pages, phy_page_count);
	printk("Phy addr 0x2A3000 page %X\n", (uint32_t)phy_to_page(0x2A3000));
	printk("Phy addr 0x2A4000 page %X\n", (uint32_t)phy_to_page(0x2A4000));
	printk("Phy addr 0x2A6000 page %X\n", (uint32_t)phy_to_page(0x2A6000));

}

struct page *phy_to_page(uint32_t addr)
{
	/* out of phy memory range */
	BUG_ON(addr < phy_page_start || addr > max_phy_addr);

	return (struct page *)((uint32_t)phy_pages + (((addr & PAGE_MASK) - phy_page_start)>>12));
}

uint32_t page_to_phy(struct page *page)
{
	BUG_ON(page - phy_pages >= phy_page_count);

	return phy_page_start + ((page - phy_pages)<<12);
}
