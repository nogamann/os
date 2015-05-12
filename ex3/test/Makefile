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
TARNAME=ex3.tar
TARSRCS=$(LIBSRC) Makefile README
RUN = ./test0
all: $(TARGETS)

$(LIBOBJ): $(LIBSRC) $(LIBSRC:.cpp=.h) ../hash.h
	$(COMPILE.cc) $% $< -o blockchain.o

$(TARGETS): $(LIBOBJ)
	ar rv libblockchain.a blockchain.o
#	$(AR) $(ARFLAGS) $@ $^
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
	cp ../libblockchain.a .
	$(CC) ./test0.o -L../ -lblockchain -lhash -lpthread -lcrypto -o test0
	$(RUN)
	
test: test0

valgrind: $(TARGETS) ./test0.o
	$(CC) ./test0.o -L../ -lblockchain -lhash -lpthread -lcrypto -o test0
	valgrind --leak-check=full --show-possibly-lost=yes \
	--fair-sched=yes --show-reachable=yes --undef-value-errors=yes $(RUN)
.PHONY: $(TARGETS) test test0 test0.o
