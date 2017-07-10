
#ifndef INCLUDE_PMM_H_
#define INCLUDE_PMM_H_

#define PAGE_SHIFT	(12)
#define PAGE_SIZE	(1 << PAGE_SHIFT)
#define PAGE_MASK	(~(PAGE_SIZE - 1))

struct page {
	uint16_t usage;
	uint16_t flags;
};

#define PG_RESERVED	0

void init_pmm();
struct page *phy_to_page(uint32_t addr);
uint32_t page_to_phy(struct page *page);

#endif
