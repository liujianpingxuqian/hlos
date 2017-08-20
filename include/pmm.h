
#ifndef INCLUDE_PMM_H_
#define INCLUDE_PMM_H_

#include <types.h>
#include <page.h>

void init_pmm();
struct page *pfn_to_page(uint32_t pfn);
struct page *phy_to_page(uint32_t addr);
uint32_t page_to_pfn(struct page *page);
uint32_t page_to_phy(struct page *page);

#endif
