AC_INIT([cienis], [1.0], [sfaragnaus@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AM_PROG_AR
AC_PROG_CC
AC_PROG_CXX
AX_CHECK_COMPILE_FLAG([-std=c++11], [CXXFLAGS="$CXXFLAGS -std=c++11"], [exit -1])
AC_PROG_INSTALL
AC_CHECK_LIB([pcsclite],[SCardConnect])
AC_SEARCH_LIBS([SCardConnect],[pcsclite])
#AC_CHECK_HEADERS([winscard.h PCSC/winscard.h],[],[AC_MSG_ERROR([Couldn't find or include winscard.h (consider installing pcsclite-dev package)])])
PKG_CHECK_MODULES(PCSC, libpcsclite > 0.20)
AC_CHECK_FUNCS([atexit])
AC_CHECK_HEADERS([stddef.h])
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
LT_INIT
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
		  Makefile
		  src/Makefile
		   ])
AC_OUTPUT
