
AC_DEFUN([BOLTHUR_SERIAL_COMMUNICATOR_SET_HOST], [
  # define templates
  AH_TEMPLATE([LINUX], [Linux build define])
  AH_TEMPLATE([WINDOWS], [Windows build define])
  AH_TEMPLATE([OSX], [Mac build define])

  # bunch of flags
  build_linux=no
  build_windows=no
  build_mac=no

  # detect the target system
  case "${host_os}" in
    linux*)
      CFLAGS="${CFLAGS} -std=c18"
      build_linux=yes
      ;;
    darwin*)
      CFLAGS="${CFLAGS} -std=c17"
      build_mac=yes
      ;;
    cygwin*|mingw*)
      CFLAGS="${CFLAGS} -std=c18"
      build_windows=yes
      ;;
    *)
      AC_MSG_ERROR(["OS $host_os is not supported"])
      ;;
  esac

  # set host defines
  if test "x$build_linux" = xyes; then
    AC_DEFINE([LINUX], [1])
  fi
  if test "x$build_windows" = xyes; then
    AC_DEFINE([WINDOWS], [1])
  fi
  if test "x$build_mac" = xyes; then
    AC_DEFINE([OSX], [1])
  fi
])
