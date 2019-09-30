
SUBDIRS = .

AUTOMAKE_OPTIONS = subdir-objects

AM_CFLAGS = -imacros $(top_builddir)/include/config.h -I$(top_srcdir)/include

bin_PROGRAMS = bolthur-communicator
bolthur_communicator_SOURCES = \
  platform/rpi/initrd.c \
  platform/rpi/kernel.c \
  initrd.c \
  kernel.c \
  main.c \
  serial.c
