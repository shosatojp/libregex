#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([libregex], [1.0.0], [me@shosato.jp])
AC_CONFIG_AUX_DIR([conf])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([src/array.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h stdbool.h stdarg.h stdio.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([memmove memset memcpy strchr free atoi])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
