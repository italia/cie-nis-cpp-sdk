if HAVE_DOXYGEN
directory = $(top_srcdir)/docs/man/man3/

dist_man_MANS = $(directory)/nis.cpp.3
$(directory)/nis.cpp.3: doxyfile.stamp

doxyfile.stamp:
	$(DOXYGEN) Doxyfile
	echo Timestamp > doxyfile.stamp

CLEANFILES = doxyfile.stamp

all-local: doxyfile.stamp

clean-local:
	rm -rf $(top_srcdir)/docs/man
	rm -rf $(top_srcdir)/docs/html
endif
