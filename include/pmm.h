
#ifndef INCLUDE_PMM_H_
#define INCLUDE_PMM_H_

#include <types.h>

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
void pmm_free_pages(uint32_t page_number);

#endif
