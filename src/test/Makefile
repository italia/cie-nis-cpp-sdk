all: tests
tests: test-nis

LINK_FLAGS = -L../../$(LIBDIR) -l$(LIBNAME) $(LDFLAGS)
CFLAGS += -I..
	
#test-interactive:
#test-verify:
test-nis:
	$(CXX) -o ../../$(BINDIR)/test-nis-cpp $(CFLAGS) example-nis-cpp.cpp $(LINK_FLAGS) 
	$(CC) -o ../../$(BINDIR)/test-nis-c $(CFLAGS) example-nis-c.c $(LINK_FLAGS) 

clean:
	rm -f *.o
	rm -f ../../$(BINDIR)/test*
