AM_CXXFLAGS=$(CXXFLAGS) -std=c++11

lib_LTLIBRARIES = libcienis.la
libcienis_la_SOURCES = nis.cpp nis_manager.cpp reader.cpp reader_pcsc.cpp requests.cpp token.cpp token_pcsc.cpp executor.h nis_manager.h reader.h reader_pcsc.h requests.h token_pcsc.h uid.h digest.cpp digest.h asn/ASNParser.cpp asn/ASNParser.h util/array.cpp  util/array.h  util/exception.cpp  util/exception.h util/util.h util/util.cpp
libcienis_la_CPPFLAGS = $(PCSC_CFLAGS) $(SSL_CFLAGS)
libcienis_la_LDFLAGS = -version.info 1:0:0 $(PCSC_LIBS) $(CRYPTO_LIBS) $(SSL_LIBS)
pkginclude_HEADERS = nis.h nis_types.h token.h

bin_PROGRAMS = testnisc testniscpp testnissod

testnisc_SOURCES = test/example-nis-c.c
nodist_EXTRA_testnisc_SOURCES = dummy.cpp
testnisc_LDADD = libcienis.la
testnisc_CPPFLAGS = $(PCSC_CFLAGS) $(SSL_CFLAGS)
testnisc_LDFLAGS = -static

testniscpp_SOURCES = test/example-nis-cpp.cpp
testniscpp_LDADD = libcienis.la
testniscpp_CPPFLAGS = $(PCSC_CFLAGS) $(SSL_CFLAGS)
testniscpp_LDFLAGS = -static

testnissod_SOURCES = test/example-nis-sod.cpp
testnissod_LDADD = libcienis.la
testnissod_CPPFLAGS = $(PCSC_CFLAGS) $(SSL_CFLAGS)
testnissod_LDFLAGS = -static
