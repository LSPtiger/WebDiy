#include <stdio.h>
#include "sys_plat.h"
#include "net.h"
#include "nlist.h"
#include "netif_pcap.h"
#include "dbg.h"
#include "nlocker.h"
#include "mblock.h"
#include "pktbuf.h"
#include "netif.h"

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
	pktbuf_t * buf = pktbuf_alloc(2000);
	pktbuf_free(buf);
	
	buf = pktbuf_alloc(2000);
	for (int i = 0; i < 3; i++)
	{
		pktbuf_add_header(buf, 33, 1);
	}
	
	for (int i = 0; i < 3; i++)
	{
		pktbuf_remove_header(buf, 33);
	}
	for (int i = 0; i < 3; i++)
	{
		pktbuf_add_header(buf, 33, 0);
	}
	
	for (int i = 0; i < 3; i++)
	{
		pktbuf_remove_header(buf, 33);
	}

	pktbuf_free(buf);

	buf = pktbuf_alloc(8);
	pktbuf_resize(buf, 32);
	pktbuf_resize(buf, 288);
	pktbuf_resize(buf, 4922);
	pktbuf_resize(buf, 1921);
	pktbuf_resize(buf, 288);
	pktbuf_resize(buf, 32);
	pktbuf_resize(buf, 0);
	pktbuf_free(buf);

	buf = pktbuf_alloc(680);
	pktbuf_t * sbuf = pktbuf_alloc(892);
	pktbuf_join(buf, sbuf);
	pktbuf_free(buf);

	buf = pktbuf_alloc(32);
    pktbuf_join(buf, pktbuf_alloc(4));
    pktbuf_join(buf, pktbuf_alloc(16));
    pktbuf_join(buf, pktbuf_alloc(54));
    pktbuf_join(buf, pktbuf_alloc(32));
	pktbuf_join(buf, pktbuf_alloc(38));

	pktbuf_set_cont(buf, 44);
	pktbuf_set_cont(buf, 60);
	pktbuf_set_cont(buf, 64);
	pktbuf_set_cont(buf, 128);
	pktbuf_set_cont(buf, 135);
	pktbuf_free(buf);

	buf = pktbuf_alloc(32);
    pktbuf_join(buf, pktbuf_alloc(4));
    pktbuf_join(buf, pktbuf_alloc(16));
    pktbuf_join(buf, pktbuf_alloc(54));
    pktbuf_join(buf, pktbuf_alloc(32));
	pktbuf_join(buf, pktbuf_alloc(38));
	pktbuf_join(buf, pktbuf_alloc(512));

	pktbuf_reset_acc(buf);
	static uint16_t temp[1000];

	for (int i = 0; i < 1000; i++)
	{
		temp[i] = i;
	}
	
	pktbuf_write(buf, (uint8_t *)temp, pktbuf_total(buf));

	pktbuf_reset_acc(buf);
	static uint16_t read_temp[1000];
	pktbuf_read(buf, (uint8_t *)read_temp, pktbuf_total(buf));
	if(plat_memcmp(temp, read_temp, pktbuf_total(buf))){
		plat_printf("not equal.");
		return;
	}
	plat_memset(read_temp, 0, sizeof(read_temp));
	pktbuf_seek(buf, 18*2);
	pktbuf_read(buf, (uint8_t *)read_temp, 56);
	if(plat_memcmp(temp + 18, read_temp, 56) != 0){
		plat_printf("not equal");
	};

	plat_memset(read_temp, 0, sizeof(read_temp));
	pktbuf_seek(buf, 85 * 2);
	pktbuf_read(buf, (uint8_t *)read_temp, 256);
	if(plat_memcmp(temp + 85, read_temp, 256) != 0){
		plat_printf("not equal");
		return;
	};

	pktbuf_t * dest = pktbuf_alloc(1024);
	pktbuf_seek(dest, 600);
	pktbuf_seek(buf, 200);
	pktbuf_copy(dest, buf, 122);

	plat_memset(read_temp, 0, sizeof(read_temp));
	pktbuf_seek(dest, 600);
	pktbuf_read(dest, (uint8_t *)read_temp, 122);
	if(plat_memcmp(temp + 100, read_temp, 122) != 0){
		plat_printf("not equal");
		return;
	}

	pktbuf_seek(dest, 0);
	pktbuf_fill(dest, 53, pktbuf_total(dest));
	plat_memset(read_temp, 0, sizeof(read_temp));
	pktbuf_seek(dest, 0);
	pktbuf_read(dest, (uint8_t *)read_temp, pktbuf_total(dest));
	char * ptr = (char *)read_temp;

	for (int i = 0; i < pktbuf_total(dest); i++)
	{
		if (*ptr++ != 53)
		{
			plat_printf("not equal.");
			return;
		}
		
	}
	


	pktbuf_free(dest);
	pktbuf_free(buf);
	

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

	netif_t * netif = netif_open("pcap");
	
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