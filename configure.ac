AC_INIT([greenfx-oanda-to-tickq], 1.m4_esyscmd([git log | grep commit | wc -l | tr -d '\n']), [anthony@atgreen.org])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

PKG_PROG_PKG_CONFIG([0.25])

AC_DEFINE_UNQUOTED(GFX_VERSION_STR,
                   ["GreenFX oanda-to-tickq $PACKAGE_VERSION"],
                   [A string containing the name and version number])

AC_DEFINE_UNQUOTED(GFX_GIT_COMMIT_HASH,
	           ["m4_esyscmd([git rev-parse HEAD | tr -d '\n'])"],
		   [A string representing the most recent git commit])

AC_PROG_CC
AC_PROG_CXX
AC_CONFIG_HEADERS([config.h])
PKG_CHECK_MODULES([AMQ], [activemq-cpp])
AC_CONFIG_FILES([
 Makefile
])
AC_OUTPUT
