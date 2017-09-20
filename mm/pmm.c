#include <multiboot.h>
#include <mm.h>
#include <pmm.h>
#include <string.h>
#include <console.h>
#include <page.h>
#include <slob.h>

/* page frame arrary start at kernel end */
static struct page *phy_pages;

/* avriable min & max phy memory for buddy system */
static uint32_t min_phy_addr;
static uint32_t max_phy_addr;
static uint32_t phy_page_count;

/* kernel space page map */
static pgd_t kern_pgd;
static pte_t kern_pte;

/* Page Frame Number to Page Struct */
struct page *pfn_to_page(uint32_t pfn)
{
	//BUG_ON(pfn >= phy_page_count);

	return phy_pages + pfn;
}

uint32_t page_to_pfn(struct page *page)
{
	return page - phy_pages;
}

/* physical address of page frame */
void *page_address(struct page *page)
{
	return __va(min_phy_addr + PFN_PHYS(page_to_pfn(page)));
}

/* map all physical memory to kernel space */
static void init_kernel_vmm(uint32_t pte_pages)
{
	uint32_t pgd_idx, idx, i;
	pgd_t *k_pgd = (pgd_t *)pgd_val(kern_pgd);
	pte_t *k_pte = (pte_t *)pte_val(kern_pte);

	pgd_idx = PGD_INDEX(PAGE_OFFSET);	/* kernel virtual address start at PAGE_OFFSET */
	for (idx = 0; idx < pte_pages; idx++) {
		uint32_t pte_addr = pte_val(kern_pte) + PFN_PHYS(idx);
		pgd_val(k_pgd[pgd_idx + idx]) = __pa(pte_addr) | PAGE_PRESENT | PAGE_WRITE;

		//printk("PGD_idx:%d map to PTE 0x%X\n", idx, pte_addr);
	}

	/* set page map to all memroy */
	for (i = 0; i < PFN_UP(max_phy_addr); i++)
		pte_val(k_pte[i])=PFN_PHYS(i) | PAGE_PRESENT | PAGE_WRITE;

	/* load new page map for kernel */
	__asm__ volatile ("mov %0, %%cr3" : : "r" (__pa(pgd_val(kern_pgd))));

}

static void show_kernel_memory_map(void)
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

static void show_memory_map(void)
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

void init_pmm()
{
	mmap_entry_t *mmap;
	uint32_t pte_pages, pfn_pages;
	uint32_t mmap_len = (uint32_t)glb_mboot_ptr->mmap_length;
	uint32_t mmap_addr = (uint32_t)glb_mboot_ptr->mmap_addr;

	/* show physical memroy map at first */
	show_memory_map();
	show_kernel_memory_map();

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
	pgd_val(kern_pgd) = (unsigned long)(__va(PFN_ALIGN(kern_end)));
	/* pte_pages PAGE_SIZE for kern_pte */
	pte_val(kern_pte) = pgd_val(kern_pgd) + PAGE_SIZE;
	pte_pages = (PFN_UP(max_phy_addr) + (PTE_SIZE - 1))/PTE_SIZE;

	/* Page Frame array start at end of kernel PTE */
	phy_pages = (struct page *)(pte_val(kern_pte) + (pte_pages << PAGE_SHIFT));
	phy_page_count = (max_phy_addr - __pa((uint32_t)phy_pages)) >> PAGE_SHIFT;
	pfn_pages = PFN_UP(phy_page_count * sizeof(struct page));
	phy_page_count -= pfn_pages; /* remove Page Frame array memroy */

	bzero(phy_pages, pfn_pages << PAGE_SHIFT);

	min_phy_addr = __pa((uint32_t)phy_pages) + PFN_PHYS(pfn_pages);

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

