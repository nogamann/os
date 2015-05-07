CC=g++
RANLIB=ranlib

LIBSRC=blockchain.cpp
LIBOBJ=$(LIBSRC:.cpp=.o)

INCS=-I.
CFLAGS = -Wall -g -rdynamic $(INCS) 
CPPFLAGS = $(CFLAGS) -std=c++11
LOADLIBES = -L./

BLOCKCHAINLIB = libblockchain.a
TARGETS = $(BLOCKCHAINLIB)

TAR=tar
TARFLAGS=-cvf
TARNAME=ex2.tar
TARSRCS=$(LIBSRC) blockchain_private.h Makefile README

all: $(TARGETS) 

$(TARGETS): $(LIBOBJ)
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@

clean:
	$(RM) $(TARGETS) $(LIBOBJ) *~ *core

depend:
	makedepend -- $(CFLAGS) -- $(SRC) $(LIBSRC)

tar:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)



test0.o: blockchain.h test0.cpp
	$(CC) -c ./test0.cpp

test0: $(TARGETS) blockchain_private.h ./test0.o
	$(CC) ./test0.o -lblockchain -lhash  -lpthread -lcrypto -L./ -o test0
	
test: test0

.PHONY: test test0 test0.o