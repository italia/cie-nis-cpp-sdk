all: tests
tests: test-nis

LINK_FLAGS = -L../../$(LIBDIR) -l$(LIBNAME) $(LDFLAGS)
CFLAGS += -I..
	
#test-interactive:
#test-verify:
test-nis:
	$(CXX) -o ../../$(BINDIR)/test-nis $(CFLAGS) example-nis.cpp $(LINK_FLAGS) 

clean:
	rm -f *.o
	rm -f ../../$(BINDIR)/test*
