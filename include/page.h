#ifndef INCLUDE_PAGE_H_
#define INCLUDE_PAGE_H_

// 内核起始虚拟地址
#define KERNBASE          (0xC0000000)

// 内核的偏移地址
#define PAGE_OFFSET 	  KERNBASE

#define PAGE_SHIFT	(12)
#define PAGE_SIZE	(1UL << PAGE_SHIFT)
#define PAGE_MASK	(~(PAGE_SIZE - 1))

#define PFN_ALIGN(x)	(((uint32_t)(x) + (PAGE_SIZE -1)) & PAGE_MASK)
#define PFN_UP(x)	(((uint32_t)(x) + PAGE_SIZE - 1) >> PAGE_SHIFT)
#define PFN_DOWN(x)	((uint32_t)(x) >> PAGE_SHIFT)
#define PFN_PHYS(x)	((uint32_t)(x) << PAGE_SHIFT)

typedef struct { unsigned long pte_low;	} pte_t;
typedef struct { unsigned long pgd;     } pgd_t;
typedef struct { unsigned long pgprot;  } pgprot_t;

#define pgd_val(x)	   ((x).pgd)
#define pte_val(x)	   ((x).pte_low)
#define pgprot_val(x)	   ((x).pgprot)
#define __pte(x) ((pte_t)  { (x) } )
#define __pgd(x) ((pgd_t)  { (x) } )
#define __pgprot(x)	   ((pgprot_t) { (x) } )


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

static inline uint32_t __pa(uint32_t x)
{
	if (x == 0)
		return 0;

	return (x - KERNBASE);
}

static inline void *__va(uint32_t x)
{
	if (x == 0)
		return (void *)0;

	return (void *)(x + KERNBASE);
}

#endif
