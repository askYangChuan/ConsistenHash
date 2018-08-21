#ifndef _CONSISTENHASH_H_
#define _CONSISTENHASH_H_

#include "ds_types.h"
#include "stlc_list.h"


typedef struct {
	struct stlc_list_head link;
	u_int32_t virtual_node_num;
	u_int32_t ip;
}server_node_t;

typedef struct {
	struct stlc_list_head link;
	server_node_t *node;
	u_int32_t pos;
}virtual_node_t;


typedef struct {
	struct stlc_list_head server_node;
	struct stlc_list_head virtual_node;
	u_int32_t server_node_num;
	u_int32_t virtual_node_num;
}server_cluster_t;



int consistenhash_node_add(u_int32_t index, u_int32_t virtual_node);
void consistenhash_node_del(u_int32_t index);
server_node_t *consistenhash_get_node(char *key);
void consistenhash_node_show();

#endif
