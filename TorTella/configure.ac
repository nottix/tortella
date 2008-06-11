#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(TorTella, 0.1, notargiacomo.s@gmail.com)
AM_INIT_AUTOMAKE(TorTella, 0.1)
AC_CONFIG_SRCDIR([src])
AM_CONFIG_HEADER(config.h)

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.
AC_PATH_XTRA
PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([gtk], [gtk+-2.0 >= 2.12.0])
PKG_CHECK_MODULES([gthread], [gthread-2.0 >= 2.0])

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netinet/in.h stdlib.h string.h sys/ioctl.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
#AM_PROG_CC_C_O

# Checks for library functions.
#AC_REPLACE_FUNCS
AC_FUNC_CLOSEDIR_VOID
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([inet_ntoa memset socket strchr strdup strstr strtoull])

AC_OUTPUT(Makefile src/Makefile)
