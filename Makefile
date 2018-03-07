SRCDIR = src
BINDIR = bin

CXX=g++
CFLAGS = -std=c++11 -g -I /usr/local/include/PCSC 
LDFLAGS = -L /usr/local/lib -lpcsclite

all:
	$(MAKE) -C $(SRCDIR)

clean:
	$(MAKE) -C $(SRCDIR) clean

export BINDIR CFLAGS LDFLAGS
install:
	$(MAKE) -C $(SRCDIR) install
