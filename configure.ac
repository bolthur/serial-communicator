
AC_PREREQ(2.57)
AC_INIT([bolthur-serial-communicator], [0.1], [https://github.com/bolthur/serial-communicator/issues], [bolthur-serial-communicator], [https://github.com/bolthur/serial-communicator])
AC_COPYRIGHT([Copyright (C) 2019 bolthur project])

AC_CONFIG_AUX_DIR([config])

AC_CANONICAL_HOST
AC_CONFIG_SRCDIR([src/main.c])
AM_INIT_AUTOMAKE([foreign 1.7])
AC_CONFIG_HEADERS([include/config.h])

AM_SILENT_RULES([yes])

AC_LANG([C])

# compile flags
CFLAGS="${CFLAGS} -Wall -Wextra -Werror -Wpedantic -Wconversion -O2"

# Detect the target system
case "${host_os}" in
  linux*)
    CFLAGS="${CFLAGS} -std=c18"
    ;;
  darwin*)
    CFLAGS="${CFLAGS} -std=c17"
    ;;
  *)
    AC_MSG_ERROR(["OS $host_os is not supported"])
    ;;
esac

# conftest
AC_LANG_CONFTEST([AC_LANG_SOURCE([[const char hw[] = "Hello, World\r\n";]])])

# checks for programs
AC_PROG_CC
AM_PROG_AS
AC_PROG_RANLIB
AM_PROG_AR
AC_PROG_CXX

AC_CONFIG_FILES([
  Makefile
  src/Makefile
])

AC_OUTPUT
