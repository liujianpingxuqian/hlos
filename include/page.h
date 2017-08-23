#ifndef INCLUDE_PAGE_H_
#define INCLUDE_PAGE_H_

// 内核起始虚拟地址
#define KERNBASE          (0xC0000000)

// 内核的偏移地址
#define PAGE_OFFSET 	  KERNBASE

#define PAGE_SHIFT	(12)
#define PAGE_SIZE	(1 << PAGE_SHIFT)
#define PAGE_MASK	(~(PAGE_SIZE - 1))

// 每个页表可以映射的内存数
#define PAGE_MAP_SIZE 	  (0x400000)

// 获取一个地址的页目录项
#define PGD_INDEX(x)      (((x) >> 22) & 0x3FF)

// 获取一个地址的页表项
#define PTE_INDEX(x)      (((x) >> 12) & 0x3FF)

// 获取一个地址的页內偏移
#define OFFSET_INDEX(x)   ((x) & 0xFFF)

// P--位0是存在标识，为 1 则内存页在内存中
#define PAGE_PRESENT 	(0x1)

// R/W--位1是读/写标识，如果等于 1，表示页面可以被读、写或执行。
#define PAGE_WRITE 	(0x2)

// U/S--位2是用户/超级用户标识，为 1 则任何特权级上的程序都可以访问该页面。
#define PAGE_USER 	(0x4)

// 页表成员数
#define PGD_SIZE (PAGE_SIZE/sizeof(pte_t))

// 页表成员数
#define PTE_SIZE (PAGE_SIZE/sizeof(uint32_t))

#define PG_RESERVED	0

struct page {
	uint16_t flags;
	uint16_t private;
	atomic_t  _count;
	atomic_t _mapcount;
	struct list_head lru;
};

struct page *pfn_to_page(uint32_t pfn);
uint32_t page_to_pfn(struct page *page);

/* API of buddy for page mangerment */
void free_pages(struct page *page, uint16_t order);
struct page *alloc_page(uint16_t order);

#endif
