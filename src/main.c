
/**
 * Copyright (C) 2019 bolthur project.
 *
 * This file is part of bolthur/serial-communicator.
 *
 * bolthur/serial-communicator is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * bolthur/serial-communicator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with bolthur/serial-communicator.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Main entry point for serial communicator
 *
 * @param argc argument count
 * @param argv argument array
 * @return int
 */
int main( int argc, char** argv ) {
  // variables
  char *device;
  char *file;

  // initial print of name and version
  printf( "%s %s\r\n", PACKAGE_NAME, PACKAGE_VERSION );

  // handle not enough parameter
  if ( 3 != argc ) {
    // print example and usage
    printf( "Example: %s /dev/ttyUSB0 kernel.img\r\n", argv[ 0 ] );
    printf( "Usage: %s <device> <file>\r\n", argv[ 0 ] );

    // exit with error
    exit( EXIT_FAILURE );
  }

  // save device and file
  device = argv[ 1 ];
  file = argv[ 2 ];

  // some output
  printf( "Try to open device \"%s\" to send file \"%s\"\r\n", device, file );

  // FIXME: Add open of serial port

  exit( EXIT_SUCCESS );
}
