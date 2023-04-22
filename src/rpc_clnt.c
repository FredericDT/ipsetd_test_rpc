#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "ipset_test_rpc.h"
#include "ipset_test.h"

#define IP4

int main(int argc, char *argv[]) {
	CLIENT *clnt;
	int *ret;
	char *server;
	char *message;
 
	if (argc != 2) {
		fprintf(stderr, "usage: %s host\n", argv[0]);
		exit(1);
	}
 
	server = argv[1];
	clnt = clnt_create(server, 
		IPSET_TEST_PROG,
		IPSET_TEST_VERS,
		"udp"
	);

	if (clnt == (CLIENT *)NULL) {
		clnt_pcreateerror(server);
		exit(1);
	}
	
#ifdef IP4
    char *setname = "a";
    int af = AF_INET;
    struct ip4_addr ipaddr = {
        .addr = 0x0100007f,
    };
#endif
	
#ifdef IP6
	char *setname = "a";
	int af = AF_INET6;
	ip6_addr ipaddr = { 
		.addr = {
			0x20, 0x01, 0x0d, 0xb8, 0x00, 0x01, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff
		}
	};
#endif
	
	struct test_ipaddr_in_ipset_req req = {
		.setname = {'\0'},
		.af = af,
	};
	memcpy(req.setname, setname, strlen(setname) + 1);

#ifdef IP4
	req.ip4addr = ipaddr;
#endif
#ifdef IP6
	req.ip6addr = ipaddr;
#endif

	ret = test_ipaddr_in_ipset_1(&req, clnt);

	if (ret == (int *)NULL) {
		clnt_perror(clnt, server);
		exit(1);
	}

	if (*ret == IPADDR_IN_IPSET) {
		fprintf(stderr, "address in ipset\n");
	}
	if (*ret == IPADDR_NOT_IN_IPSET) {
		fprintf(stderr, "address NOT in ipset\n");
	}
	if (*ret == EXIT_FAILURE) {
		perror("test_ipaddr_in_ipset");
	}

	clnt_destroy(clnt);

    return EXIT_SUCCESS;
}