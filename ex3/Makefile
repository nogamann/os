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
TARNAME=ex3.tar
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
