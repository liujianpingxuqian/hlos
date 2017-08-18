
#ifndef INCLUDE_PMM_H_
#define INCLUDE_PMM_H_

#include <types.h>

#define PAGE_SHIFT	(12)
#define PAGE_SIZE	(1 << PAGE_SHIFT)
#define PAGE_MASK	(~(PAGE_SIZE - 1))

struct page {

	uint16_t flags;
	uint16_t private;
	atomic_t  _count;
	atomic_t _mapcount;
	struct list_head lru;
};

#define PG_RESERVED	0

void init_pmm();
struct page *pfn_to_page(uint32_t pfn);
struct page *phy_to_page(uint32_t addr);
uint32_t page_to_pfn(struct page *page);
uint32_t page_to_phy(struct page *page);

#endif
