#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <time.h>

#include "ipset_test_rpc.h"
#include "ipset_test.h"

int * test_ipaddr_in_ipset_1_svc(test_ipaddr_in_ipset_req *req, struct svc_req *svc_req) {
    static int ret = 0;
    ret = test_ipaddr_in_ipset_init();
    if (ret) {
        return  (&ret);
    }
    ret = test_ipaddr_in_ipset_svc(
        req->setname, 
        req->af, 
        req->af == AF_INET ? &(req->ip4addr) : &(req->ip6addr)
    );
    fprintf(stderr, "%ld: setname = %s, af = %d, ", time(NULL), req->setname, req->af);
    if (req->af == AF_INET) {
        fprintf(stderr, "ipaddr = ");
        for (int i = 0; i < 4; ++i) {
            fprintf(stderr, "%02x", ((char *)&(req->ip4addr.addr))[i]);
        }
        fprintf(stderr, "\n");
    } else if (req->af == AF_INET6) {
        fprintf(stderr, "ipaddr = ");
        for (int i = 0; i < 16; ++i) {
            fprintf(stderr, "%02x", req->ip6addr.addr[i]);
        }
        fprintf(stderr, "\n");
    }
    test_ipaddr_in_ipset_deinit();
    return (&ret);
}