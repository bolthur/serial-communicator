
SUBDIRS = .

AUTOMAKE_OPTIONS = subdir-objects

AM_CFLAGS = -imacros $(top_builddir)/config.h -I$(top_srcdir)/include

bin_PROGRAMS = bolthur-communicator
bolthur_communicator_SOURCES = \
  main.c \
  kernel.c \
  serial.c
