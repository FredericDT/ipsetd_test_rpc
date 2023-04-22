#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/utsname.h>
#include <arpa/inet.h>
#include <linux/version.h>
#include <linux/netlink.h>
#include <linux/netfilter/nfnetlink.h>
#include <linux/netfilter/ipset/ip_set.h>

#include "ipset_test.h"

static struct mnl_socket *nl;
static unsigned int portid;

int callback(const struct nlmsghdr *nlh, void *data) {
	int ret = MNL_CB_ERROR;
	const struct nlmsgerr *err = mnl_nlmsg_get_payload(nlh);
	fprintf(stderr, "err->error = %d\n", err->error);
	if (err->error == 0) {
		ret = MNL_CB_STOP;
		/* address in ipset */
	}
	if (err->error == -IPSET_ERR_EXIST) {
		/* address NOT in ipset */
	}
	return ret;
}

int test_ipaddr_in_ipset(
	struct mnl_socket *nl, 
	unsigned int portid, 
	char *setname,
	int af,
	void *ipaddr
) {
	struct nlmsghdr *nlh;
	struct nfgenmsg *nfg;
	struct nlattr *nested[2];
	char buffer[MNL_SOCKET_BUFFER_SIZE];
	int ret;

	if (strlen(setname) >= IPSET_MAXNAMELEN) {
		errno = ENAMETOOLONG;
		return EXIT_FAILURE;
	}
	if (af != AF_INET && af != AF_INET6) {
		errno = EAFNOSUPPORT;
		return EXIT_FAILURE;
	}

	nlh = mnl_nlmsg_put_header(buffer);
	nlh->nlmsg_type = (NFNL_SUBSYS_IPSET << 8) | IPSET_CMD_TEST;
	nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;

    nfg = mnl_nlmsg_put_extra_header(nlh, sizeof(struct nfgenmsg));
	nfg->nfgen_family = af;
	nfg->version = NFNETLINK_V0;
	nfg->res_id = htons(0);

    mnl_attr_put_u8(nlh, IPSET_ATTR_PROTOCOL, IPSET_PROTOCOL);
	mnl_attr_put(nlh, IPSET_ATTR_SETNAME, strlen(setname) + 1, setname);
	nested[0] = mnl_attr_nest_start(nlh, IPSET_ATTR_DATA);
	nested[1] = mnl_attr_nest_start(nlh, IPSET_ATTR_IP);
	mnl_attr_put(nlh, (af == AF_INET ? IPSET_ATTR_IPADDR_IPV4 : IPSET_ATTR_IPADDR_IPV6)
			| NLA_F_NET_BYTEORDER, (af == AF_INET ? sizeof(struct in_addr) : sizeof(struct in6_addr)), ipaddr);
	mnl_attr_nest_end(nlh, nested[1]);
	mnl_attr_nest_end(nlh, nested[0]);

    if (mnl_socket_sendto(nl, nlh, nlh->nlmsg_len) < 0) {
        perror("mnl_socket_sendto");
		return EXIT_FAILURE;
	}

    buffer[0] = '\0';
    
    ret = mnl_socket_recvfrom(nl, buffer, MNL_SOCKET_BUFFER_SIZE);

    if (ret == -1) {
        perror("mnl_socket_recvfrom");
        return EXIT_FAILURE;
    }

    while (ret > 0) {
		ret = mnl_cb_run(buffer, ret, 0, portid, callback, NULL);

		if (ret == MNL_CB_STOP) {
			return IPADDR_IN_IPSET;
		}

		if (ret == MNL_CB_ERROR) {
			return IPADDR_NOT_IN_IPSET;
		}
        
        if (ret < 0) {
			fprintf(stderr, "ret = %d\n", ret);
            perror("mnl_cb_run");
            return EXIT_FAILURE;
        }
		
        ret = mnl_socket_recvfrom(nl, buffer, MNL_SOCKET_BUFFER_SIZE);
        if (ret == -1) {
            perror("mnl_socket_recvfrom");
            return EXIT_FAILURE;
        }
    }
}

int init_mnl_socket(struct mnl_socket **nl, unsigned int *portid) {
	*nl = mnl_socket_open(NETLINK_NETFILTER);
    if (*nl == NULL) {
		perror("mnl_socket_open");
		return EXIT_FAILURE;
	}

    if (mnl_socket_bind(*nl, 0, MNL_SOCKET_AUTOPID) < 0) {
        perror("mnl_socket_bind");
		return EXIT_FAILURE;
    }
    *portid = mnl_socket_get_portid(*nl);
	return EXIT_SUCCESS;
}

int test_ipaddr_in_ipset_svc(
	char *setname,
	int af,
	void *ipaddr
) {
    int ret = test_ipaddr_in_ipset(nl, portid, setname, af, ipaddr);
	return ret;
}

int test_ipaddr_in_ipset_init() {
	if (init_mnl_socket(&nl, &portid)) {
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int test_ipaddr_in_ipset_deinit() {
	return mnl_socket_close(nl);
}