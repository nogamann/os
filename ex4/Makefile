make: CachingFileSystem

CC=g++

SRC=CachingFileSystem.cpp CachingManager.c log.c
OBJ=CachingFileSystem.o CachingManager.o log.o

INCS=-I.
CPPFLAGS = -Wall -D_FILE_OFFSET_BITS=64 $(INCS)

TARGETS = CachingFileSystem

TAR=tar
TARFLAGS=-cvf
TARNAME=ex4.tar
TARSRCS=$(SRC) CachingFileSystem.h Makefile README

all: $(TARGETS) 

$(TARGETS): $(OBJ)
	$(CC) $(CPPFLAGS) `pkg-config fuse --cflags --libs` $(OBJ) -o CachingFileSystem

clean:
	$(RM) $(TARGETS) $(OBJ) *~ *core

depend:
	makedepend -- $(CPPFLAGS) -- $(SRC)

tar:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)
