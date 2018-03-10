all: objs tests
tests: test-nis 
#	test-interactive 
#	test-verify
objs: example-nis.o 
#	example-interactive.o 
#	example-verify.o
	
test-interactive:
	$(CXX) -o ../../$(BINDIR)/test-interactive example-interactive.o ../requests.o $(LDFLAGS)

test-nis:
	$(CXX) -o ../../$(BINDIR)/test-nis example-nis.o ../requests.o ../nis.o ../token.o ../reader.o ../reader_pcsc.o ../token_pcsc.o $(LDFLAGS)

test-verify:
	$(CXX) -o ../../$(BINDIR)/test-verify example-verify.o ../requests.o $(LDFLAGS)

example-interactive.o:
	$(CXX) $(CFLAGS) -I.. -c example-interactive.cpp

example-nis.o:
	$(CXX) $(CFLAGS) -I.. -c example-nis.cpp

example-verify.o:
	$(CXX) $(CFLAGS) -I.. -c example-verify.cpp

clean:
	rm -f *.o
	rm -f ../../$(BINDIR)/test*
