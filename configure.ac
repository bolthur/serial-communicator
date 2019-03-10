
AC_PREREQ(2.57)
AC_INIT([bolthur-serial-communicator], [0.1], [bug-serial-communicator@bolthur.org])
AC_COPYRIGHT([Copyright (C) 2019 bolthur project])

AC_CONFIG_SRCDIR([main.c])
AC_CONFIG_AUX_DIR([config])

AC_CANONICAL_HOST
AC_CONFIG_SRCDIR([src/main.c])
AM_INIT_AUTOMAKE([foreign 1.7])
AC_CONFIG_HEADERS([config.h])

AC_LANG([C])

AC_ARG_ENABLE([opt],
  AC_HELP_STRING([--disable-opt], [disable code optimization (default: on)]),
  [case "${enableval}" in
  no | 0)
    CFLAGS="${CFLAGS} -O0"
    ;;
  1)
    CFLAGS="${CFLAGS} -O1"
    ;;
  2)
    CFLAGS="${CFLAGS} -O2"
    ;;
  3)
    CFLAGS="${CFLAGS} -O3"
    ;;
  s)
    CFLAGS="${CFLAGS} -Os"
    ;;
  g)
    CFLAGS="${CFLAGS} -Og"
    ;;
  *)
    CFLAGS="${CFLAGS} -O2"
    ;;
  esac],
  [CFLAGS="${CFLAGS} -O2"]
)

# compile flags
CFLAGS="${CFLAGS} -Wall -Wextra -Werror -Wpedantic -Wconversion"

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
