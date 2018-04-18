AC_INIT([cienis], [1.0], [sfaragnaus@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AM_PROG_AR
AC_PROG_CC
AC_PROG_CXX
#AX_CHECK_COMPILE_FLAG([-std=c++11], [CXXFLAGS="$CXXFLAGS -std=c++11"], [exit -1])
AC_PROG_INSTALL
AC_CHECK_LIB([pcsclite],[SCardConnect])
AC_SEARCH_LIBS([SCardConnect],[pcsclite])
AC_CHECK_LIB([crypto],[RAND_bytes])
AC_SEARCH_LIBS([RAND_bytes],[crypto])
AC_CHECK_LIB([ssl],[d2i_X509])
AC_SEARCH_LIBS([d2i_X509],[ssl])
#AC_CHECK_HEADERS([winscard.h PCSC/winscard.h],[],[AC_MSG_ERROR([Couldn't find or include winscard.h (consider installing pcsclite-dev package)])])
PKG_CHECK_MODULES(PCSC, libpcsclite > 0.20)
PKG_CHECK_MODULES(SSL, libssl > 1.0)
PKG_CHECK_MODULES(CRYPTO, libcrypto > 1.0)

AC_MSG_CHECKING([for libcrypto++])
AC_LANG(C++)
SAVED_LIBS=$LIBS
LIBS="-lcrypto++"
AC_LINK_IFELSE([AC_LANG_PROGRAM([#include <crypto++/sha.h>], [CryptoPP::SHA256 dummy])], 
	[AC_MSG_RESULT([found])
	HAVE_CRYPTO=1;], 
	[AC_MSG_ERROR([libcrypto++ is not installed.])
	HAVE_CRYPTO=0;])
LIBS=$SAVED_LIBS
if test $HAVE_CRYPTO = 1; then
	LIBS="$LIBS -lcrypto++"
fi

AC_CHECK_PROGS([DOXYGEN], [doxygen])
	if test -z "$DOXYGEN";
		   then AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
	fi
	AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([docs/Doxyfile docs/Makefile])])
AC_CHECK_FUNCS([atexit])
AC_CHECK_HEADERS([stddef.h])
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_SUBST([LIBS])
LT_INIT
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
		  Makefile
		  src/Makefile
		   ])
AC_OUTPUT
