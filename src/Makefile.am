lib_LTLIBRARIES = libcienis.la
libcienis_la_SOURCES = nis.cpp nis_manager.cpp reader.cpp reader_pcsc.cpp requests.cpp token.cpp token_pcsc.cpp executor.h nis_manager.h reader.h reader_pcsc.h requests.h token_pcsc.h uid.h
libcienis_la_CPPFLAGS = $(PCSC_CFLAGS)
libcienis_la_LDFLAGS = -version.info 0:0:0 $(PCSC_LIBS)
include_HEADERS = nis.h nis_types.h token.h

bin_PROGRAMS = testnisc testniscpp

testnisc_SOURCES = test/example-nis-c.c
testnisc_LDADD = libcienis.la
testnisc_CPPFLAGS = $(PCSC_CFLAGS)

testniscpp_SOURCES = test/example-nis-cpp.cpp
testniscpp_LDADD = libcienis.la
testniscpp_CPPFLAGS = $(PCSC_CFLAGS)
