MAJOR_VERSION = 1
MINOR_VERSION = 0
PATCHLEVEL = 1
LIBNAME = cienis
PLATFORM = posix

SRCDIR = src
BINDIR = bin
LIBDIR = lib

MKDIR = mkdir -p
CXX = g++-4.9
CFLAGS = -std=c++11 -g -I/usr/local/include/PCSC -I/usr/include/PCSC
LDFLAGS = -L/usr/local/lib -lpcsclite
ifeq ($(PLATFORM),posix)
	LDFLAGS += -lpthread
endif

export CXX BINDIR LIBDIR CFLAGS LDFLAGS LIBNAME MAJOR_VERSION MINOR_VERSION PATCHLEVEL PLATFORM

all:
	$(MKDIR) $(LIBDIR) $(BINDIR)
	$(MAKE) -C $(SRCDIR)

clean:
	$(MAKE) -C $(SRCDIR) clean

install:
	$(MAKE) -C $(SRCDIR) install
