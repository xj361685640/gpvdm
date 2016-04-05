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
],
  [AC_MSG_ERROR("No assembler code for CPU $target_os")]
)
#i?86
#x86_64

m4_include([make_files.m4])

AC_CHECK_HEADERS(math.h)
AC_FUNC_MALLOC
AC_CHECK_FUNCS([sqrt])


#AC_CONFIG_FILES
#images/Makefile

m4_include([config_files.m4])

AC_OUTPUT

AC_MSG_NOTICE([gpvdm build finished.])
