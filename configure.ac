AC_INIT([gpvdm], [4.80])
AC_CONFIG_SRCDIR([./src/main.c])
AC_CANONICAL_TARGET
AC_CANONICAL_HOST
#AM_GNU_GETTEXT([external])

AC_MSG_NOTICE([$target])
AC_MSG_NOTICE([$target_alias])
AC_MSG_NOTICE([$target_cpu])
AC_MSG_NOTICE([$target_vendor])
AC_MSG_NOTICE([$target_os])
AC_ARG_ENABLE(noimages, "used to disable compilation of images",
[AM_CONDITIONAL([INCLUDE_IMAGES],[false])],
[AM_CONDITIONAL([INCLUDE_IMAGES],[true])]
)

AC_ARG_ENABLE(noplots, "used to disable plot files",
[AM_CONDITIONAL([INCLUDE_PLOTS],[false])],
[AM_CONDITIONAL([INCLUDE_PLOTS],[true])]
)

AC_ARG_ENABLE(nopman, "used to disable man page generation",
[AM_CONDITIONAL([INCLUDE_MAN],[false])],
[AM_CONDITIONAL([INCLUDE_MAN],[true])]
)

AC_SUBST(WIN_ICON, "" )
AC_SUBST(WIN_RES_FILE, "" )

#This is a case statment the default is to compile for linux
AS_CASE([$target_os],
  [mingw32], [

AC_MSG_NOTICE([********************************])
AC_MSG_NOTICE([*Setting up config for windows *])
AC_MSG_NOTICE([********************************])

AC_SUBST(LINK_FLAG, "-posix")
AC_SUBST(COMPILE_FLAG, " -D windows -posix")
AC_SUBST(SLIB_EXT, "a")
AC_SUBST(DLIB_EXT, "dll")
AC_SUBST(EXE_EXT, ".exe")

AC_CHECK_HEADERS([umfpack.h],[],[AC_MSG_ERROR([umfpack.h is needed but not found])])
AC_SUBST(DLL_LINKER, ["-L/home/rod/windll/compiled_dlls/ -Wl,--out-implib,plugin.a -lgpvdm_light  -lgpvdm_lib -lgpvdm_dump -lgpvdm_measure -lgpvdm_dos -lgpvdm_dumpctrl -lgpvdm_light -lgpvdm_i -lzip-2 -lumfpack -lgpvdm_mesh "])
AC_SUBST(DLL_COMPILE, ["-DBUILDING_EXAMPLE_DLL"])
AC_SUBST(DLL_DEBUG, ["-Wall"])
AC_SUBST(EXE_LINK,["-L/home/rod/windll/compiled_dlls/ -lumfpack -lzip-2"])
AC_SUBST(WIN_RES_FILE, "gpvdm.res" )
AC_SUBST(WIN_ICON, "i686-w64-mingw32-windres gpvdm.rc -O coff -o gpvdm.res" )

], [

AC_MSG_NOTICE([*****************************])
AC_MSG_NOTICE([*Setting up config for linux*])
AC_MSG_NOTICE([*****************************])
AC_SUBST(LINK_FLAG, "")
AC_CHECK_LIB(zip, [main], [], [AC_MSG_ERROR([libzip is needed but not found.])])
AC_CHECK_HEADERS([suitesparse/umfpack.h],[],[AC_MSG_ERROR([suitesparse/umfpack.h is needed but not found])])
AC_SUBST(SLIB_EXT, "a")
AC_SUBST(DLIB_EXT, "so")
AC_SUBST(EXE_EXT, "")


AC_SUBST(DLL_LINKER, ["`dpkg-buildflags --get LDFLAGS` -Wl,-soname,plugin.so"])
AC_SUBST(DLL_COMPILE, ["-fPIC"])
AC_SUBST(DLL_DEBUG, ["-Wall -g"])



AC_ARG_ENABLE(hpc, "used to turn on HPC compatability",
  [
AC_MSG_NOTICE([*****************************])
AC_MSG_NOTICE([*Setting up for HPC         *])
AC_MSG_NOTICE([*****************************])

AC_SUBST(COMPILE_FLAG, " -D enable_server -g -fPIC " )
AC_SUBST(EXE_LINK, " -lumfpack -lm -lgsl -lgslcblas -rdynamic -export-dynamic -ldl -lzip -lz  -L/home/steve/rm/lib/")
#-lmatheval -g -L/home/steve/rm/build/libmatheval-1.1.11/lib/.libs/

],[
	AC_CHECK_LIB(matheval, [main], [], [AC_MSG_ERROR([matheval is needed but not found.])])
	AC_SUBST(COMPILE_FLAG, " -D enable_server -g -fPIC `pkg-config --cflags dbus-1` -D dbus " )
	AC_SUBST(EXE_LINK, "-lumfpack -lm -lgsl -lgslcblas -rdynamic -export-dynamic -ldl -lzip  -lmatheval -lz -g `pkg-config --libs dbus-1`")
	
]
)


]
)


AC_PROG_RANLIB
AM_INIT_AUTOMAKE
AC_PROG_CC

AC_ARG_ENABLE(hpc, "used to turn on HPC compatability",
  [ ],[

	AM_PATH_PYTHON([2.7], [], [AC_MSG_ERROR([You need python installed])])

	AC_CHECK_PROG([PKG_HELP2MAN], help2man, help2man,no)
	if test "$PKG_HELP2MAN" = "no"; then
		AC_MSG_ERROR([help2man is not installed.])
	fi

	AC_CHECK_PROG([PKG_RSGV], rsvg-convert, rsvg-convert,no)
	if test "$PKG_RSGV" = "no"; then
		AC_MSG_ERROR([rsvg-convert is not installed.])
	fi

]
)

AC_CHECK_HEADERS([zip.h],[],[AC_MSG_ERROR([zip.h is needed but not found])])
AC_CHECK_HEADERS([gsl/gsl_multimin.h],[],[AC_MSG_ERROR([gsl/gsl_multimin.h is needed but not found])])

AC_CHECK_PROG([PKG_CONFIG], pkg-config, pkg-config,no)
if test "$PKG_CONFIG" = "no"; then
	AC_MSG_ERROR([pkg-config is not installed.])
fi




#i?86
#x86_64

m4_include([make_files.m4])
m4_include([local_link.m4])

m4_include([ar.m4])

AC_CHECK_HEADERS(math.h)
AC_FUNC_MALLOC
AC_CHECK_FUNCS([sqrt])


#AC_CONFIG_FILES

m4_include([config_files.m4])

AC_OUTPUT

AC_MSG_NOTICE([gpvdm build finished.])
