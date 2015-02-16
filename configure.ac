#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
VERSION=0.10
AC_INIT(libral2, 0.10, ari.jolma_at_gmail.com)
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_HEADERS([include/priv/config.h])
AC_CONFIG_FILES([Makefile libral2.pc include/ral2/config.h])
AC_CONFIG_MACRO_DIR([m4])

AC_SUBST(REQUIRES)
AC_SUBST(PC_LIBS)
AC_SUBST(PC_CPPFLAGS)
AC_SUBST(LIBS)
AC_SUBST(CPPFLAGS)
AC_SUBST(HAVE_GDAL)
AC_SUBST(HAVE_GDK_PIXBUF)

# Checks for programs.
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

case "$target_os" in
cygwin*)
    LDFLAGS = "$LDFLAGS -no-undefined"
    ;;
esac

# Checks for byteorder, typedefs, structures, and compiler characteristics.
AC_C_BIGENDIAN
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([float.h limits.h stddef.h stdlib.h string.h unistd.h math.h errno.h string.h stdarg.h],,
		 [AC_MSG_FAILURE(
			[One of required standard headers NOT found.])])

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
LIBS="$LIBS -lm"
AC_CHECK_FUNCS([floor pow sqrt strchr strerror],,
               [AC_MSG_FAILURE(
                   [One of required standard functions NOT found.])])
AC_CHECK_FUNCS([strdup])

AC_MSG_CHECKING([for local path])

CPPFLAGS="$CPPFLAGS -I./include"

AC_ARG_WITH(local,[  --with-local[[=DIR]]      the local path [[default=/usr/local]]],,)
if test "$with_local" != "no"; then
  if test "$with_local" = "yes" -o "$with_local" = ""; then
    ADD_PREFIX=/usr/local
  else
    ADD_PREFIX=$with_local
  fi
  LIBS="$LIBS -L$ADD_PREFIX/lib"
  CPPFLAGS="$CPPFLAGS -I$ADD_PREFIX/include"
  AC_MSG_RESULT(using $ADD_PREFIX)
else
  AC_MSG_RESULT([none])
fi

PKG_CHECK_MODULES(GDK_PIXBUF, gdk-pixbuf-2.0, gdk=true)
if test "$gdk" = "true"; then
  LIBS="$LIBS $GDK_PIXBUF_LIBS"
  CPPFLAGS="$CPPFLAGS $GDK_PIXBUF_CFLAGS"
  PC_CPPFLAGS=""
  REQUIRES="gdk-pixbuf-2.0"
else
  AC_MSG_FAILURE([gdk-pixbuf not found!])
fi

PKG_CHECK_MODULES(PANGOCAIRO, pangocairo, pangocairo=true)
if test "$pangocairo" = "true"; then
  LIBS="$LIBS $PANGOCAIRO_LIBS"
  CPPFLAGS="$CPPFLAGS $PANGOCAIRO_CFLAGS"
  PC_CPPFLAGS=""
  REQUIRES="pangocairo"
else
  AC_MSG_FAILURE([pangocairo not found!])
fi

AC_ARG_WITH(gdal,[  --with-gdal=PATH        set path to gdal-config [[default is without explicit path]]],,)
if test "$with_gdal" = "" ; then
  with_gdal="gdal-config"
fi
TMP=`$with_gdal --libs`
LIBS="$LIBS $TMP"
PC_LIBS="$PC_LIBS $TMP"
TMP=`$with_gdal --cflags`
CPPFLAGS="$CPPFLAGS $TMP"
PC_CPPFLAGS="$PC_CPPFLAGS $TMP"
AC_CHECK_HEADER(gdal.h, gdal=true)
if test "$gdal" = "true"; then
  REQUIRES="$REQUIRES"
else
  AC_MSG_FAILURE([GDAL not found!])
fi

AC_MSG_CHECKING([whether to add debugging support])
AC_ARG_WITH(debug,[  --with-debug            whether to add debug code [[default=no]]],,)
if test "$with_debug" = "yes"; then
  CPPFLAGS="-g $CPPFLAGS"
  AC_MSG_RESULT(yes)
else
  CPPFLAGS="-O2 $CPPFLAGS"
  AC_MSG_RESULT(no)
fi

AC_OUTPUT
