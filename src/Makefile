all: objs tests
tests: objs
	$(MAKE) -C test

objs: requests.o nis.o token.o reader.o reader_pcsc.o token_pcsc.o
	
requests.o:
	$(CXX) $(CFLAGS) -c requests.cpp

nis.o:
	$(CXX) $(CFLAGS) -c nis.cpp

token.o:
	$(CXX) $(CFLAGS) -c token.cpp

reader.o:
	$(CXX) $(CFLAGS) -c reader.cpp

token_pcsc.o:
	$(CXX) $(CFLAGS) -c token_pcsc.cpp

reader_pcsc.o:
	$(CXX) $(CFLAGS) -c reader_pcsc.cpp

clean:
	rm -f *.o
	$(MAKE) -C test clean
