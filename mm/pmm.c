#include <multiboot.h>
#include <debug.h>
#include <printk.h>
#include <pmm.h>
#include <string.h>
#include <page.h>
#include <slob.h>

/* page frame arrary start at kernel end */
static struct page *phy_pages;

/* avriable min & max phy memory for buddy system */
static uint32_t min_phy_addr;
static uint32_t max_phy_addr;
static uint32_t phy_page_count;

/* kernel space page map */
static pgd_t *kern_pgd;
static pte_t *kern_pte;

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

/* physical memory address of page frame */
uint32_t page_to_addr(struct page *page)
{
	return min_phy_addr + page_to_pfn(page) * PAGE_SIZE;
}

/* page frame of physical memory address */
struct page *addr_to_page(uint32_t pa)
{
	uint32_t pfn = ((pa & PAGE_MASK) - min_phy_addr) >> PAGE_SHIFT;

	return pfn_to_page(pfn);
}

/* map all physical memory to kernel space */
static void init_kernel_vmm(uint32_t pte_pages)
{
	uint32_t pgd_idx, idx, i;

	pgd_idx = PGD_INDEX(PAGE_OFFSET);
	for (idx = 0; idx < pte_pages; idx++) {
		uint32_t pte_addr = (uint32_t)kern_pte + idx * PAGE_SIZE;
		kern_pgd[pgd_idx + idx] = va_to_pa(pte_addr) | PAGE_PRESENT | PAGE_WRITE;

		//printk("PGD_idx:%d map to PTE 0x%X\n", idx, pte_addr);
	}

	/* set page map to all memroy */
	for (i = 0; i < (max_phy_addr >> PAGE_SHIFT); i++)
		kern_pte[i] = (i<<PAGE_SHIFT) | PAGE_PRESENT | PAGE_WRITE;

	/* load new page map for kernel */
	__asm__ volatile ("mov %0, %%cr3" : : "r" (va_to_pa((uint32_t)kern_pgd)));

}

void init_pmm()
{
	mmap_entry_t *mmap;
	uint32_t pte_pages, pfn_pages;
	uint32_t mmap_len = (uint32_t)glb_mboot_ptr->mmap_length;
	uint32_t mmap_addr = (uint32_t)glb_mboot_ptr->mmap_addr;

	for (mmap = (mmap_entry_t *) mmap_addr; (uint32_t)mmap < mmap_addr + mmap_len; mmap++ ) {
		/* manage avriable memory above 1MB, and one blank */
		if ((mmap->type == 1) && (mmap->base_addr_low >= 0x100000)) {
			max_phy_addr = (mmap->base_addr_low + mmap->length_low) & PAGE_MASK;
			//phy_page_count = mmap->length_low / PAGE_SIZE;
			//phy_page_count = max_phy_addr / PAGE_SIZE;
		}
	}

	/* memory map
	 * end of Page_Frame should be less than 16M as we only map 16M in temporatory before
	 *
	 * |---1M---|------kern-------|---kern_pte---|--phy_pages--|---------PAGE------|
	 *   empty                          PTE           frame          4K..4K
	 *
	 * PGD --> PTE  --> 0 ~ MAX(128M)      (PTE for memroy access of MMU)
	 * Page_Frame  --> min_phy_addr ~ max_phy_addr  (PFN for memory mallocable)
	 */

	/* one PAGE_SIZE for kern_pgd */
	kern_pgd = (pgd_t *)((pa_to_va((uint32_t)kern_end) + PAGE_SIZE - 1) & PAGE_MASK);
	/* pte_pages PAGE_SIZE for kern_pte */
	kern_pte = (pte_t *)((uint32_t)kern_pgd + PAGE_SIZE);
	pte_pages = ((max_phy_addr / PAGE_SIZE) + PTE_SIZE - 1)/PTE_SIZE;

	/* Page Frame array start at end of kernel PTE */
	phy_pages = (struct page *)((uint32_t)kern_pte + pte_pages * PAGE_SIZE);
	phy_page_count = (pa_to_va(max_phy_addr) - (uint32_t)phy_pages)/PAGE_SIZE;
	pfn_pages = (phy_page_count * sizeof(struct page) + PAGE_SIZE -1)/PAGE_SIZE;
	phy_page_count -= pfn_pages; /* remove Page Frame array memroy */

	bzero(phy_pages, pfn_pages * PAGE_SIZE);

	min_phy_addr = (uint32_t)phy_pages + pfn_pages * PAGE_SIZE;
	min_phy_addr = va_to_pa(min_phy_addr);

	printk("Avriable Physic Memory 0x%08X ~~~ 0x%08X\n", min_phy_addr, max_phy_addr);
	printk("Page Frame start 0x%08X with %d pages, total count %d\n", phy_pages, pfn_pages, phy_page_count);

	/* free pages to buddy system */
	pmm_free_pages(phy_page_count);

	/* reload kernel page map */
	init_kernel_vmm(pte_pages);

	/* init slob allocment */
	slob_init();

	//show_free_area();
}

