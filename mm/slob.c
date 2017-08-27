/*
 * =====================================================================================
 *
 *       Filename:  slob.c
 *
 *    Description:  slob 内存分配器
 *
 *        Version:  1.0
 *        Created:  2014年11月11日 15时15分41秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#include <list.h>
#include <page.h>
#include <slob.h>
#include <printk.h>

typedef
struct slob_block {
        uint32_t allocated : 1;     // 该内存块是否已经被申请
        uint32_t length : 31;       // 当前内存块的长度
        struct list_head list;
} slob_block_t;

#define SLOB_USED    1
#define SLOB_FREE    0

// slob 管理的内存数量
#define SLOB_PAGE_ORDER  10

// slob 最小的内存分片限制
#define SLOB_MIN_PART    0x20

// 初始化头结点
LIST_HEAD(slob_head);

// 简化 list_entry 使用
#define le_to_block(le) list_entry(le, slob_block_t, list)

static void *__slob_alloc_pages(uint32_t order)
{
        uint32_t addr;
	struct page *page;

	page = alloc_page(order);
	addr = page_to_addr(page);

        return (void *)pa_to_va(addr);
}

static void slob_print(void)
{
        struct list_head *le = NULL;

        list_for_each(le, &slob_head) {
                slob_block_t *block = le_to_block(le);
                printk("Addr: %08X  length: %d  used: %d\n", block, block->length, block->allocated);
        }
        printk("\n");
}

static void slob_test(void)
{
        slob_print();
 
        void *addr1 = kmalloc(100);
        slob_print();
	kfree(addr1);
        slob_print();
        addr1 = kmalloc(100);
        void *addr2 = kmalloc(200);
        void *addr3 = kmalloc(300);
        kfree(addr2);
        kfree(addr3);
        slob_print();
        void *addr4 = kmalloc(2220);
        void *addr5 = kmalloc(240);
        void *addr6 = kmalloc(2230);
        kfree(addr6);
        kfree(addr1);
        slob_print();
        void *addr7 = kmalloc(220);
        void *addr8 = kmalloc(1200);

        kfree(addr4);
        kfree(addr8);
        slob_print();
        kfree(addr7);
        kfree(addr5);
        //kfree(addr6);

        slob_print();
}

// slob 分配器初始化
void slob_init(void)
{
        slob_block_t *block = __slob_alloc_pages(SLOB_PAGE_ORDER);

        block->allocated = SLOB_FREE;
        block->length = (1 << SLOB_PAGE_ORDER) * PAGE_SIZE - sizeof(slob_block_t);
        list_add(&block->list, &slob_head);

        //slob_test();
}

// 切分内存块
static void split_chunk(slob_block_t *chunk_block, uint32_t len)
{
	slob_block_t *new_chunk;
	uint32_t left = chunk_block->length - len;

	/* must allow another chunk at left */
	if (left < sizeof(slob_block_t) + SLOB_MIN_PART)
		return;

	new_chunk = (slob_block_t *)((uint32_t)chunk_block + len);
	new_chunk->allocated = SLOB_FREE;
	new_chunk->length = left;

	list_add(&new_chunk->list, &chunk_block->list);

	chunk_block->length = len - sizeof(slob_block_t);
	chunk_block->allocated = SLOB_USED;
}

// 合并内存块
static void glue_chunk(slob_block_t *chunk_block)
{
	slob_block_t *prev_block, *next_block;
	struct list_head *prev = chunk_block->list.prev;
	struct list_head *next = chunk_block->list.next;

        if (prev != &slob_head) {
                prev_block = le_to_block(prev);
                if (prev_block->allocated == SLOB_FREE) {
                        prev_block->length += (chunk_block->length + sizeof(slob_block_t));
                        list_del(&chunk_block->list);
                        chunk_block = prev_block;
                }
        }

        if (next != &slob_head) {
                next_block = le_to_block(next);
                if (next_block->allocated == SLOB_FREE) {
                        chunk_block->length += (next_block->length + sizeof(slob_block_t));
                        list_del(&next_block->list);
                }
        }
}

// 任意大小的内存分配函数
void *kmalloc(uint32_t size)
{
	struct list_head *le = NULL;
	uint32_t len = (size > SLOB_MIN_PART) ? size : SLOB_MIN_PART;
        
	len += sizeof(slob_block_t);

	list_for_each(le, &slob_head) {
		slob_block_t *block = le_to_block(le);
		if (block->allocated == SLOB_FREE && block->length > len) {
			split_chunk(block, len);
			void *addr = (void *)((uint32_t)block + sizeof(slob_block_t));
			return addr;
		} 
        }

        return NULL;
}

// 任意大小的内存释放函数
void kfree(void *addr)
{
        slob_block_t *block = (slob_block_t *)((uint32_t)addr - sizeof(slob_block_t));

        if (block->allocated != SLOB_USED)
                return;

        block->allocated = SLOB_FREE;
        glue_chunk(block);
}

