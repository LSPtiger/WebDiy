#include <stdio.h>
#include "sys_plat.h"
#include "net.h"
#include "nlist.h"
#include "netif_pcap.h"
#include "dbg.h"
#include "nlocker.h"
#include "mblock.h"
#include "pktbuf.h"
static sys_sem_t read_sem, write_sem;
static int count;
static sys_mutex_t mutex;
static char buffer[100];
static int write_index, read_index, total;

typedef struct _tnode_t
{
	int id;
	nlist_node_t node;
} tnode_t;

void nlist_test(void)
{
#define NODE_COUNT 3

	tnode_t tnodes[NODE_COUNT];
	nlist_t list;
	nlist_init(&list);
	for (int i = 0; i < NODE_COUNT; i++)
	{
		tnodes[i].id = i;
		nlist_insert_head(&list, &tnodes[i].node);
	}

	nlist_node_t *p;

	plat_printf("head:\n");
	nlist_for_each(p, &list)
	{
		tnode_t *tnode = nlist_entry(p, tnode_t, node);
		plat_printf("%d\n", tnode->id);
	}

	plat_printf("remove head:\n");
	for (int i = 0; i < NODE_COUNT; i++)
	{
		p = nlist_remove_head(&list);
		plat_printf("%d \n", nlist_entry(p, tnode_t, node)->id);
	}

	plat_printf("tail insert:\n");
	for (int i = 0; i < NODE_COUNT; i++)
	{
		tnodes[i].id = i;
		nlist_insert_tail(&list, &tnodes[i].node);
	}
	nlist_for_each(p, &list)
	{
		tnode_t *tnode = nlist_entry(p, tnode_t, node);
		plat_printf("%d\n", tnode->id);
	}

	// plat_printf("remove tail:\n");
	// for (int i = 0; i < NODE_COUNT; i++)
	// {
	// 	p = nlist_remove_head(&list);
	// 	plat_printf("%d \n", nlist_entry(p, tnode_t, node)->id);
	// }


	plat_printf("insert after:\n");
	for (int i = 0; i < NODE_COUNT; i++)
	{
		tnodes[i].id = i;
		nlist_insert_after(&list, nlist_head(&list), &tnodes[i].node);
	}

	nlist_for_each(p, &list)
	{
		tnode_t *tnode = nlist_entry(p, tnode_t, node);
		plat_printf("%d\n", tnode->id);
	}
}

void pktbuf_test(void){
	pktbuf_t * buf = pktbuf_alloc(336);
	pktbuf_free(buf);

	buf = pktbuf_alloc(2000);
	for (int i = 0; i < 16; i++)
	{
		pktbuf_add_header(buf, 33, 1);
	}
	
	for (int i = 0; i < 16; i++)
	{
		pktbuf_remove_header(buf, 33);
	}
	
}

void mblock_test(void){
	mblock_t blist;
	static uint8_t buffer[10][100];
	mblock_init(&blist, buffer, 100, 10,NLOCKER_THREAD);

	void * temp[10];
	for (int i = 0; i < 10; i++)
	{
		temp[i] = mblock_alloc(&blist, 0);
		plat_printf("block: %p, free_count: %d\n", temp[i], mblock_free_cnt(&blist));
	}

	for (int i = 0; i < 10; i++)
	{
		mblock_free(&blist, temp[i]);
		plat_printf("free count: %d \n", mblock_free_cnt(&blist));
	}
	
	mblock_destroy(&blist);
}
void basic_test(void){
	nlist_test();
	mblock_test();
	pktbuf_test();

	
}
net_err_t netdev_init(void)
{
	netif_pcap_open();
	return NET_ERR_OK;
}

int main(void)
{
	
	dbg_warning(DBG_LEVEL_INFO, "dbginfo:%s", "ababa");
	dbg_error(DBG_LEVEL_ERROR, "dbgerror:%s", "error", "one more string");
	dbg_assert(2 < 1, "error");
	net_init();
	pktbuf_test();

	net_start();

	netdev_init();

	while (1)
	{
		sys_sleep(10);
	}

	return 0;
}