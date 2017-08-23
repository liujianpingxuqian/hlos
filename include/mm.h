#ifndef INCLUDE_MM_H_
#define INCLUDE_MM_H_

#include <types.h>
#include <page.h>

#include <atomic.h>

/*
 * Setup the page count before being freed into the page allocator for
 * the first time (boot or memory hotplug)
 */
static inline void init_page_count(struct page *page)
{
	atomic_set(&page->_count, 1);
}

static inline int page_count(struct page *page)
{
	return atomic_read(&page->_count);
}

static inline void page_mapcount_reset(struct page *page)
{
	atomic_set(&(page)->_mapcount, -1);
}

static inline int page_mapcount(struct page *page)
{
	return atomic_read(&(page)->_mapcount) + 1;
}

static inline void get_page(struct page *page)
{
	/*
	 * Getting a normal page or the head of a compound page
	 * requires to already have an elevated page->_count.
	 */
	atomic_inc(&page->_count);
}

static inline void set_page_count(struct page *page, int v)
{
	atomic_set(&page->_count, v);
}

/*
 * Drop a ref, return true if the refcount fell to zero (the page has no users)
 */
static inline int put_page_testzero(struct page *page)
{
	return atomic_dec_and_test(&page->_count);
}


/* Buddy flags */
#define PAGE_BUDDY_MAPCOUNT_VALUE (-128)

static inline int PageBuddy(struct page *page)
{
	return atomic_read(&page->_mapcount) == PAGE_BUDDY_MAPCOUNT_VALUE;
}

static inline void __SetPageBuddy(struct page *page)
{
	atomic_set(&page->_mapcount, PAGE_BUDDY_MAPCOUNT_VALUE);
}

static inline void __ClearPageBuddy(struct page *page)
{
	atomic_set(&page->_mapcount, -1);
}

#endif
