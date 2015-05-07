CC=g++
RANLIB=ranlib

LIBSRC=../blockchain.cpp
LIBOBJ=$(LIBSRC:.cpp=.o)


INCS=-I.
CFLAGS = -Wall -g -std=c++11 $(INCS)
CPPFLAGS = -Wall -g -std=c++11
LOADLIBES = -L./

COMPILE.cc=$(CC) $(CPPFLAGS) -c

OSMLIB = libblockchain.a
TARGETS = $(OSMLIB)

TAR=tar
TARFLAGS=-cvf
TARNAME=ex2.tar
TARSRCS=$(LIBSRC) Makefile README

all: $(TARGETS)

$(LIBOBJ): $(LIBSRC) $(LIBSRC:.cpp=.h) ../hash.h
	$(COMPILE.cc) $% $<

$(TARGETS): $(LIBOBJ)
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@

clean:
	$(RM) $(TARGETS) $(OSMLIB) $(OBJ) $(LIBOBJ) *~ *core

depend:
	makedepend -- $(CFLAGS) -- $(SRC) $(LIBSRC)

tar:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)

test0.o: ../blockchain.h ../hash.h ./test0.cpp
	$(CC) -c ./test0.cpp

test0: $(TARGETS) ./test0.o
	$(CC) ./test0.o $(TARGETS) -lhash  -lpthread -L./ -lcrypto -o test0
	./test0
	
test: test0

.PHONY: $(TARGETS) test test0 test0.o
