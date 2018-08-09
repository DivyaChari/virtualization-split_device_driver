CC=/opt/hermit/bin/x86_64-hermit-gcc
SRC=userspace.c
PROG=userspace
CFLAGS= -g -pthread
LDFLAGS=
PROXY=/opt/hermit/bin/proxy
MEM=1G

all: $(PROG)
	$(CC) $(CFLAGS) $(SRC) -o $(PROG) $(LDFLAGS)

test: all
	HERMIT_MEM=$(MEM) HERMIT_ISLE=uhyve HERMIT_KVM=0 $(PROXY) $(PROG)
