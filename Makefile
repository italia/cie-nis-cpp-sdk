MAJOR_VERSION = 1
MINOR_VERSION = 0
PATCHLEVEL = 1
LIBNAME = cienis
PLATFORM = posix

SRCDIR = src
BINDIR = bin
LIBDIR = lib
DOCDIR = doc

MKDIR = mkdir -p
CXX = g++
CC = gcc
CFLAGS = -std=c++11 -g -I/usr/local/include/PCSC -I/usr/include/PCSC
LDFLAGS = -L/usr/local/lib -lpcsclite
ifeq ($(PLATFORM),posix)
	LDFLAGS += -lpthread
endif

export CXX CC BINDIR LIBDIR DOCDIR CFLAGS LDFLAGS LIBNAME MAJOR_VERSION MINOR_VERSION PATCHLEVEL PLATFORM

all:
	$(MKDIR) $(LIBDIR) $(BINDIR) $(DOCDIR)
	$(MAKE) -C $(SRCDIR)

clean:
	$(MAKE) -C $(SRCDIR) clean
	rm -rf doc/*

install:
	$(MAKE) -C $(SRCDIR) install

.PHONY: doc
doc:
	doxygen Doxyfile
