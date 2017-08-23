#include <multiboot.h>
#include <debug.h>
#include <printk.h>
#include <pmm.h>
#include <string.h>
#include <page.h>

/* page frame arrary start at kernel end */
static struct page *phy_pages;
static uint32_t phy_page_count;

/* avriable min & max phy memory */
static uint32_t min_phy_addr;
static uint32_t max_phy_addr;


/* Page Frame Number to Page Struct */
struct page *pfn_to_page(uint32_t pfn)
{
	BUG_ON(pfn >= phy_page_count);

	return phy_pages + pfn;
}

uint32_t page_to_pfn(struct page *page)
{
	return page - phy_pages;
}

void init_pmm()
{
	mmap_entry_t *mmap;
	uint32_t mmap_len = (uint32_t)glb_mboot_ptr->mmap_length;
	uint32_t mmap_addr = (uint32_t)glb_mboot_ptr->mmap_addr;

	for (mmap = (mmap_entry_t *) mmap_addr; (uint32_t)mmap < mmap_addr + mmap_len; mmap++ ) {
		/* manage avriable memory above 1MB, and one blank */
		if ((mmap->type == 1) && (mmap->base_addr_low >= 0x100000)) {
			max_phy_addr = mmap->base_addr_low + mmap->length_low;
			phy_page_count = mmap->length_low / PAGE_SIZE;
		}
	}

	phy_pages = (struct page *)(((uint32_t)kern_end + PAGE_SIZE) & PAGE_MASK);
	bzero(phy_pages, phy_page_count * sizeof(struct page));

	min_phy_addr = ((uint32_t)phy_pages + phy_page_count * sizeof(struct page) + PAGE_SIZE) & PAGE_MASK;

	printk("Avriable Mem_phy 0x%08X ~~~ 0x%08X\n", min_phy_addr, max_phy_addr);
	printk("Page Frame start 0x%08X, total count %d\n", phy_pages, phy_page_count);

	/* free pages to buddy system */
	pmm_free_pages(phy_page_count);
}

