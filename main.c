#include <stdio.h>
#include <netinet/in.h>

#include "ConsistenHash.h"

struct server_info_s {
	char *ip;
	u_int32_t virtual_node;
};


struct server_info_s servers[] = {
	{"10.10.9.11", 8},
	{"10.10.9.12", 6},
	{"10.10.9.13", 5},
	{"10.10.9.14", 3},
};

static inline u_int32_t str_to_ip(char *str)
{

	u_int32_t ip = 0;
	struct in_addr addr;
	
	if(inet_aton(str, &addr) != 0){
		ip = htonl(addr.s_addr);
	}
	return ip;
}

int main()
{
	int i = 0;
	consistenhash_init();

	for(i = 0; i < sizeof(servers)/sizeof(struct server_info_s); i++) {
		consistenhash_node_add(str_to_ip(servers[i].ip), servers[i].virtual_node);
	}

	consistenhash_node_show();

    return 0;
}