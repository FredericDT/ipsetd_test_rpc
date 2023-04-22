struct ip4_addr {
    uint32_t addr;
};
struct ip6_addr {
    unsigned char addr[16];
};

struct test_ipaddr_in_ipset_req {
    char setname[32];
	int af;
	ip4_addr ip4addr; 
    ip6_addr ip6addr; 
};

program IPSET_TEST_PROG {
    version IPSET_TEST_VERS {
        int TEST_IPADDR_IN_IPSET(test_ipaddr_in_ipset_req) = 1;
    } = 1;
} = 0x200000f1;