
AC_PREREQ(2.57)
AC_INIT([bolthur-serial-communicator], [0.1], [https://github.com/bolthur/serial-communicator/issues], [bolthur-serial-communicator], [https://github.com/bolthur/serial-communicator])
AC_COPYRIGHT([Copyright (C) 2019 - 2020 bolthur project])

AC_CONFIG_AUX_DIR([build-aux/config])
AC_CONFIG_MACRO_DIR([build-aux/m4])

AC_CANONICAL_HOST
AC_CONFIG_SRCDIR([src/main.c])
AM_INIT_AUTOMAKE()
AC_CONFIG_HEADERS([include/config.h])

AC_LANG([C])

# Set flag and host
BOLTHUR_SERIAL_COMMUNICATOR_SET_FLAG
BOLTHUR_SERIAL_COMMUNICATOR_SET_HOST

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
