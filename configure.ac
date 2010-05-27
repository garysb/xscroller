# Ensure we have the correct version of autoconf running and initialise it
AC_PREREQ([2.61])
AC_INIT(xscroller, 0.9, xscroller@stroppytux.net, ,http://www.stroppytux.net/projects/xscroller/)
AM_PROG_LIBTOOL
AC_CONFIG_MACRO_DIR([m4])

# Set some information about the headers and makefiles
AC_CONFIG_SRCDIR([src/xscroller.h])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_FILES([Makefile src/Makefile])

# Checks for programs.
AM_INIT_AUTOMAKE
AC_PROG_CC

# Checks for header files and make sure ease has ease_multi
AC_PATH_X
AC_CHECK_HEADERS([Xlib.h Xutil.h Xft.h shape.h Imlib.h xmlmemory.h parser.h stdio.h string.h stdlib.h unistd.h pthread.h semaphore.h],,AC_MSG_ERROR([required header file missing]))
AC_HEADER_STDC
AC_EGREP_HEADER(ease_multi, ease.h, AC_DEFINE(HAVE_EASE_MULTI, 1, [Define to 1 if you have ease_multi]))

# Check for the libease, X11 and Xmu libraries
AC_CHECK_LIB(ease, ease)
AC_CHECK_LIB(X11, XOpenDisplay)
AC_CHECK_LIB(Xmu, XmuClientWindow)

AC_OUTPUT

