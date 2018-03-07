all: objs tests
tests: objs
	$(MAKE) -C test

objs: requests.o
	
requests.o:
	$(CXX) $(CFLAGS) -c requests.cpp

clean:
	rm -f *.o
	$(MAKE) -C test clean
