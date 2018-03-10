MAJOR_VERSION = 1
MINOR_VERSION = 0
PATCHLEVEL = 1
LIBNAME = cienis

SRCDIR = src
BINDIR = bin
LIBDIR = lib

CXX = g++
CFLAGS = -std=c++11 -g -I/usr/local/include/PCSC 
LDFLAGS = -L/usr/local/lib -lpcsclite
MKDIR = mkdir -p

export BINDIR LIBDIR CFLAGS LDFLAGS LIBNAME MAJOR_VERSION MINOR_VERSION PATCHLEVEL

all:
	$(MKDIR) $(LIBDIR) $(BINDIR)
	$(MAKE) -C $(SRCDIR)

clean:
	$(MAKE) -C $(SRCDIR) clean

install:
	$(MAKE) -C $(SRCDIR) install
