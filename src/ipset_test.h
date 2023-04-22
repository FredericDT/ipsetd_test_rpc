#pragma once

#include <libmnl/libmnl.h>

#define IPADDR_IN_IPSET 0xf1
#define IPADDR_NOT_IN_IPSET 0xf2

extern int init_mnl_socket(struct mnl_socket **nl, unsigned int *portid);
extern int test_ipaddr_in_ipset(
	struct mnl_socket *nl, 
	unsigned int portid, 
	char *setname,
	int af,
	void *ipaddr
);
extern int test_ipaddr_in_ipset_svc(
	char *setname,
	int af,
	void *ipaddr
);
extern int test_ipaddr_in_ipset_init();
extern int test_ipaddr_in_ipset_deinit();