#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "ConsistenHash.h"
#include "MurmurHash3.h"

#define MURMURHASH_SEED 17

#define IP_FORMART "%u.%u.%u.%u"
#define IP_SHOW(ip) (((ip) >> 24)&0xFF), (((ip) >> 16)&0xFF), (((ip) >> 8)&0xFF), ((ip)&0xFF)


static server_cluster_t *server_cluster = NULL;


void virtual_node_show(virtual_node_t *vnode)
{
	printf("virtual_node: "IP_FORMART", virtual_node pos: %u\n", IP_SHOW(vnode->node->ip), vnode->pos);
}

void add_virtual_node(virtual_node_t *vnode)
{
	virtual_node_t *pos;
	if(stlc_list_empty(&server_cluster->virtual_node)) {
		stlc_list_add_tail(&vnode->link, &server_cluster->virtual_node);
		server_cluster->virtual_node_num++;
		return;
	}

	pos = stlc_list_entry(server_cluster->virtual_node.prev, virtual_node_t, link);
	if(pos->pos < vnode->pos) {
		stlc_list_add_tail(&vnode->link, &server_cluster->virtual_node);
		server_cluster->virtual_node_num++;
		return;
	}

	stlc_list_for_each_entry(pos, &server_cluster->virtual_node, link) {
		if(pos->pos > vnode->pos) {
			stlc_list_add_tail(&vnode->link, &pos->link);
			server_cluster->virtual_node_num++;
			break;
		}
	}
	return;
}

virtual_node_t *find_virtual_node(u_int32_t pos)
{
	virtual_node_t *v;

	if(stlc_list_empty(&server_cluster->virtual_node)) {
		return NULL;
	}

	v = stlc_list_entry(server_cluster->virtual_node.prev, virtual_node_t, link);
	if(v->pos < pos) {
		return v;
	}
	stlc_list_for_each_entry(v, &server_cluster->virtual_node, link) {
		if(v->pos > pos) {
			return v;
		}
	}
}

void server_node_show(server_node_t *node)
{
	printf("server_node: "IP_FORMART", virtual_node_num: %u\n", IP_SHOW(node->ip), node->virtual_node_num);
}

void server_node_free(server_node_t *node)
{
	virtual_node_t *pos, *n;
	if(node->virtual_node_num > 0) {
		stlc_list_for_each_entry_safe(pos, n, &server_cluster->virtual_node, link) {
			if(pos->node == node) {
				stlc_list_del(&pos->link);
				free(pos);
				server_cluster->virtual_node_num--;
			}
		}
	}

	stlc_list_del(&node->link);
	server_cluster->server_node_num--;
	free(node);
}

server_node_t *server_node_lookup(u_int32_t pos)
{
	virtual_node_t *vnode;
	server_node_t *node;

	vnode = find_virtual_node(pos);
	if(vnode) {
		return vnode->node;
	}
	return NULL;
}

int consistenhash_node_add(u_int32_t ip, u_int32_t virtual_node)
{
	char domain[100] = {0};
	u_int32_t i = 0, n;
	assert(server_cluster);
	server_node_t *node = calloc(1, sizeof(server_node_t));
	assert(node);
	
	node->ip = ip;
	node->virtual_node_num = virtual_node;
	stlc_list_add_tail(&node->link, &server_cluster->server_node);
	server_cluster->server_node_num++;

	for(i = 0; i < virtual_node; i++) {
		memset(domain, 0, sizeof(domain));
		virtual_node_t *vnode = calloc(1, sizeof(virtual_node_t));
		assert(vnode);
		
		vnode->node = node;
		n = sprintf(domain, "SERVER_"IP_FORMART"_NODE_%u", IP_SHOW(ip), i);
		MurmurHash3_x86_32(domain, n, MURMURHASH_SEED, &vnode->pos);
		add_virtual_node(vnode);
	}
}

void consistenhash_node_del(u_int32_t ip)
{
	server_node_t *pos, *n;

	stlc_list_for_each_entry_safe(pos, n, &server_cluster->server_node, link) {
		if(pos->ip == ip) {
			server_node_free(pos);
			return;
		}
	}
}

server_node_t *consistenhash_get_node(char *key)
{
	u_int32_t pos;

	MurmurHash3_x86_32(key, strlen(key), MURMURHASH_SEED, &pos);

	return server_node_lookup(pos);
}

void consistenhash_node_show()
{
	server_node_t *n; 
	virtual_node_t *v;
	stlc_list_for_each_entry(n, &server_cluster->server_node, link) {
		server_node_show(n);
	}

	stlc_list_for_each_entry(v, &server_cluster->virtual_node, link) {
		virtual_node_show(v);
	}	
	printf("server node: %u, virtual node: %u\n", server_cluster->server_node_num, server_cluster->virtual_node_num);
}



int consistenhash_init()
{
	server_cluster = calloc(1, sizeof(server_cluster_t));
	assert(server_cluster);

	STLC_INIT_LIST_HEAD(&server_cluster->server_node);
	STLC_INIT_LIST_HEAD(&server_cluster->virtual_node);
	return 0;
}


