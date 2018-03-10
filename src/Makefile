all: $(LIBNAME) tests
tests: $(LIBNAME)
	$(MAKE) -C test

OBJS = requests.o nis.o token.o reader.o reader_pcsc.o token_pcsc.o
	
.cpp.o:
	$(CXX) -fPIC $(CFLAGS) -c $<

$(LIBNAME):$(OBJS)
	$(CXX) -shared -fPIC $(LDFLAGS) -Wl,-soname,lib$@.so.$(MAJOR_VERSION) -o ../$(LIBDIR)/lib$@.so.$(MAJOR_VERSION).$(MINOR_VERSION).$(PATCHLEVEL) $(OBJS)
	ln -sf ../$(LIBDIR)/lib$@.so.$(MAJOR_VERSION).$(MINOR_VERSION).$(PATCHLEVEL) ../$(LIBDIR)/lib$@.so.$(MAJOR_VERSION)
	ln -sf ../$(LIBDIR)/lib$@.so.$(MAJOR_VERSION).$(MINOR_VERSION).$(PATCHLEVEL) ../$(LIBDIR)/lib$@.so

clean:
	rm -f *.o
	rm -f ../$(LIBDIR)/*.so*
	$(MAKE) -C test clean
