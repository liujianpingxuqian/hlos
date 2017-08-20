
#ifndef INCLUDE_PMM_H_
#define INCLUDE_PMM_H_

#include <types.h>
#include <page.h>

/* kernel image segments memmap */
extern uint8_t kern_start[];
extern uint8_t kern_init_text_start[];
extern uint8_t kern_init_text_end[];
extern uint8_t kern_init_data_start[];
extern uint8_t kern_init_data_end[];
extern uint8_t kern_text_start[];
extern uint8_t kern_text_end[];
extern uint8_t kern_data_start[];
extern uint8_t kern_data_end[];
extern uint8_t kern_end[];

void init_pmm();
struct page *pfn_to_page(uint32_t pfn);
struct page *phy_to_page(uint32_t addr);
uint32_t page_to_pfn(struct page *page);
uint32_t page_to_phy(struct page *page);

#endif
