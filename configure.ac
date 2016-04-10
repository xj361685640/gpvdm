AC_INIT([gpvdm], [4.40])
AC_CONFIG_SRCDIR([./src/main.c])
AC_CANONICAL_TARGET
AC_CANONICAL_HOST


AC_PROG_RANLIB
AM_PROG_AR
AM_INIT_AUTOMAKE
AC_PROG_CC

AC_SUBST(WIN_LINK_FLAGS, "-D roderick")


AS_CASE([$target_os],
  [mingw32], [
AC_SUBST(LINK_FLAG, "-posix")
AC_SUBST(COMPILE_FLAG, "-I\$(HOME)/windll/libzip/libzip-0.11.2/lib/ -D windows -posix")
],
  [linux-gnu], [
AC_SUBST(LINK_FLAG, "")
AC_SUBST(COMPILE_FLAG, "`pkg-config --cflags dbus-1` -D enable_server")
AC_SUBST(LDFLAGS, "`dpkg-buildflags --get LDFLAGS`")
],
  [AC_MSG_ERROR("No assembler code for CPU $target_os")]
)

AM_PATH_PYTHON([2.7], [], [AC_MSG_ERROR([You need python installed])])

AC_CHECK_HEADERS([zip.h],[],[AC_MSG_ERROR([zip.h is needed but not found])])

AC_CHECK_LIB(zip, [main], [], [AC_MSG_ERROR([libzip is needed but not found.])])


AC_CHECK_HEADERS([suitesparse/umfpack.h],[],[AC_MSG_ERROR([suitesparse/umfpack.h is needed but not found])])
AC_CHECK_HEADERS([gsl/gsl_multimin.h],[],[AC_MSG_ERROR([gsl/gsl_multimin.h is needed but not found])])
AC_CHECK_LIB(matheval, [main], [], [AC_MSG_ERROR([matheval is needed but not found.])])

AC_CHECK_PROG([PKG_CONFIG], pkg-config, pkg-config,no)
if test "$PKG_CONFIG" = "no"; then
	AC_MSG_ERROR([pkg-config is not installed.])
fi

AC_CHECK_PROG([PKG_HELP2MAN], help2man, help2man,no)
if test "$PKG_HELP2MAN" = "no"; then
	AC_MSG_ERROR([help2man is not installed.])
fi

AC_CHECK_PROG([PKG_IMAGEMAGICK], convert, convert,no)
if test "$PKG_IMAGEMAGICK" = "no"; then
	AC_MSG_ERROR([imagemagick is not installed.])
fi

#i?86
#x86_64

m4_include([make_files.m4])
m4_include([local_link.m4])

AC_CHECK_HEADERS(math.h)
AC_FUNC_MALLOC
AC_CHECK_FUNCS([sqrt])


#AC_CONFIG_FILES
#images/Makefile

m4_include([config_files.m4])

AC_OUTPUT

AC_MSG_NOTICE([gpvdm build finished.])
