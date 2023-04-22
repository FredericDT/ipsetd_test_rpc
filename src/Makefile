.DEFAULT_GOAL := all

CC=gcc
CFLAGS=-I.

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

rpc_ipset_test_svc: ipset_test_rpc_proc.o ipset_test.o ipset_test_rpc_svc.o ipset_test_rpc_xdr.o
	$(CC) -o $@ ipset_test_rpc_proc.o ipset_test.o ipset_test_rpc_svc.o ipset_test_rpc_xdr.o -lmnl

rpc_ipset_test_clnt: ipset_test_rpc_clnt.o rpc_clnt.o ipset_test_rpc_xdr.o
	$(CC) -o $@ ipset_test_rpc_clnt.o rpc_clnt.o ipset_test_rpc_xdr.o

# rpcgen: ipset_test_rpc.x
# 	rpcgen ipset_test_rpc.x

clean: 
	rm *.o rpc_ipset_test_svc rpc_ipset_test_clnt

all: rpc_ipset_test_svc rpc_ipset_test_clnt