#include <list.h>
#include <atomic.h>
#include <page.h>
#include <mm.h>
#include <console.h>

#define MAX_ORDER	11

struct free_area {
	struct list_head  free_list;
	uint32_t nr_free;
};

struct buddy_mm {
	struct free_area	free_area[MAX_ORDER];	
	uint32_t free_pages;
	uint32_t count_pages;
};

/* only one zone in our system */
static struct buddy_mm sys_zone;

#define page_private(page)		((page)->private)
#define set_page_private(page, v)	((page)->private = (v))


static inline uint16_t page_order(struct page *page)
{
	return page_private(page);
}

static inline void set_page_order(struct page *page, uint16_t order)
{
	set_page_private(page, order);
	__SetPageBuddy(page);
}

static inline void rmv_page_order(struct page *page)
{
	__ClearPageBuddy(page);
	set_page_private(page, 0);
}

/*
 * Locate the struct page for both the matching buddy in our
 * pair (buddy1) and the combined O(n+1) page they form (page).
 *
 * 1) Any buddy B1 will have an order O twin B2 which satisfies
 * the following equation:
 *     B2 = B1 ^ (1 << O)
 * For example, if the starting buddy (buddy2) is #8 its order
 * 1 buddy is #10:
 *     B2 = 8 ^ (1 << 1) = 8 ^ 2 = 10
 *
 * 2) Any buddy B will have an order O+1 parent P which
 * satisfies the following equation:
 *     P = B & ~(1 << O)
 *
 * Assumption: *_mem_map is contiguous at least up to MAX_ORDER
 */
static inline unsigned int
__find_buddy_index(unsigned int page_idx, uint16_t order)
{
	return page_idx ^ (1 << order);
}

static inline int page_is_buddy(struct page *buddy, uint16_t order)
{
	if (PageBuddy(buddy) && page_order(buddy) == order)
		return 1;
	return 0;
}

static inline void __free_one_page(struct buddy_mm *zone, struct page *page, uint16_t order)
{
	unsigned int page_idx;
	unsigned int combined_idx;
	unsigned int buddy_idx;
	struct page *buddy;

	page_idx = page_to_pfn(page) & ((1 << MAX_ORDER) - 1);

	while (order < MAX_ORDER-1) {
		buddy_idx = __find_buddy_index(page_idx, order);
		buddy = page + (buddy_idx - page_idx);
		if (!page_is_buddy(buddy, order))
			break;

		list_del(&buddy->lru);
		zone->free_area[order].nr_free--;
		rmv_page_order(buddy);

		combined_idx = buddy_idx & page_idx;
		page = page + (combined_idx - page_idx);
		page_idx = combined_idx;
		order++;
	}
	set_page_order(page, order);
	list_add(&page->lru, &zone->free_area[order].free_list);
	zone->free_area[order].nr_free++;
}

static inline void expand(struct buddy_mm *zone, struct page *page,
	int low, int high, struct free_area *area)
{
	unsigned int size = 1 << high;

	while (high > low) {
		area--;
		high--;
		size >>= 1;

		list_add(&page[size].lru, &area->free_list);
		area->nr_free++;
		set_page_order(&page[size], high);
	}
}

/*
 * Go through the free lists and remove
 * the smallest available page from the freelists
 */
static inline
struct page *__rmqueue_smallest(struct buddy_mm *zone, unsigned int order)
{
	unsigned int current_order;
	struct free_area * area;
	struct page *page;

	/* Find a page of the appropriate size in the preferred list */
	for (current_order = order; current_order < MAX_ORDER; ++current_order) {
		area = &(zone->free_area[current_order]);
		if (list_empty(&area->free_list))
			continue;

		page = list_entry(area->free_list.next, struct page, lru);
		list_del(&page->lru);
		rmv_page_order(page);
		area->nr_free--;
		expand(zone, page, order, current_order, area);
		return page;
	}

	return NULL;
}

void free_pages(struct page *page, uint16_t order)
{
	if (put_page_testzero(page))
		__free_one_page(&sys_zone, page, order);
}

struct page *alloc_pages(uint16_t order)
{
	struct page *page;

	page = __rmqueue_smallest(&sys_zone, order);
	if (!page)
		return NULL;

	set_page_private(page, 0);
	set_page_count(page, 1);

	return page;
}

void show_free_area(void)
{
	struct buddy_mm *zone = &sys_zone;
	struct free_area *free_area;
	uint32_t nr[MAX_ORDER];
	uint32_t total = 0, order;

	for (order = 0; order < MAX_ORDER; order++) {
		struct page *page;
		free_area = &zone->free_area[order];

		if (free_area->nr_free == 0)
			continue;

		nr[order] = free_area->nr_free;
		total += nr[order] << order;

		printk("Order(%d) %dKB at ", order, nr[order]<<(order + 2));
		list_for_each_entry(page, &free_area->free_list, lru)
			printk("0x%X ", page_to_pfn(page));
		printk("\n");
	}

	printk("Total Free Memory %dMB\n", total>>8);
}

static void init_buddy_mm(void)
{
	int i;
	struct buddy_mm *zone = &sys_zone;

	zone->free_pages = 0;
	zone->count_pages = 0;

	for (i = 0; i < MAX_ORDER; i++) {
		zone->free_area[i].nr_free = 0;
		INIT_LIST_HEAD(&zone->free_area[i].free_list);
	}
}

void pmm_free_pages(uint32_t page_number)
{
	uint32_t p_idx = 0;
	uint16_t p_order = MAX_ORDER - 1;

	/* init buddy struct */
	init_buddy_mm();

	/* init pages at first */
	for (p_idx = 0; p_idx < page_number; p_idx++) {
		struct page *page;

		page = pfn_to_page(p_idx);
		init_page_count(page);
		page_mapcount_reset(page);
		INIT_LIST_HEAD(&page->lru);
	}
	printk("Free %d Pages to Buddy\n", page_number);

	/* free total pages to buddy */
	p_idx = 0;
	while(p_idx < page_number) {
		uint32_t p_left = page_number - p_idx;
		if (p_left >= (1<<p_order)) {
			free_pages(pfn_to_page(p_idx), p_order);	
			//printk("Free page[0x%X] with order %d\n", p_idx, p_order);
			p_idx += 1<<p_order;
		} else
			p_order --;
	}

	/* show buddy system status */
	show_free_area();
}

