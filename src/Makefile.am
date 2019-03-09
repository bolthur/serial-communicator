
SUBDIRS = .

AUTOMAKE_OPTIONS = subdir-objects

AM_CFLAGS = -imacros $(top_builddir)/config.h -I$(top_srcdir)/include

bin_PROGRAMS = bolthur-communicator
bolthur_communicator_SOURCES = \
  vendor/rpi/kernel.c \
  kernel.c \
  main.c \
  serial.c
