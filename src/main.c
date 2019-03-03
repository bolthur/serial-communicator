
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
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

#include "serial.h"
#include "kernel.h"

/**
 * @brief Serial handle
 */
static serial_handle_t handle = 0;

/**
 * @brief file buffer
 */
static uint8_t* file_buffer = NULL;

/**
 * @brief Previous structure
 */
static struct termios previous;

/**
 * @brief Cleanup method on exit
 */
void cleanup( void ) {
  // close serial handle
  serial_close( handle );

  // free file buffer if set
  if ( NULL != file_buffer ) {
    free( file_buffer );
  }

  // restore settings for STDIN_FILENO
  if ( isatty( STDIN_FILENO ) ) {
    tcsetattr( STDIN_FILENO, TCSANOW, &previous );
  }
}

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
  bool finished = false;

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

  // register exit function
  atexit( cleanup );

  // save device and file
  device = argv[ 1 ];
  file = argv[ 2 ];

  // load file to transfer
  printf( "Try to load file \"%s\" for transfer\r\n", file );
  file_buffer = kernel_load( file );
  if ( NULL == file_buffer ) {
    exit( EXIT_FAILURE );
  }

  // set stdin to non blocking and unbuffered
  if ( 0 > fcntl( STDIN_FILENO, F_SETFL, O_NONBLOCK ) ) {
    fprintf( stderr, "Unable to set stdin to non blocking and unbuffered!" );
    exit( EXIT_FAILURE );
  }

  // handling if stdin is a tty
  if ( isatty( STDIN_FILENO ) ) {
    // get current options
    if ( 0 > tcgetattr( STDIN_FILENO, &previous ) ) {
      fprintf( stderr, "Unable to get attributes of current tty!" );
      exit( EXIT_FAILURE );
    }

    // get temporary copy
    struct termios new_termios = previous;

    // disable canonical mode (buffered I/O) and local echo
    new_termios.c_lflag = ( tcflag_t )( ( int32_t )new_termios.c_lflag & ( ~ICANON & ~ECHO ) );

    // write back changes
    if ( 0 > tcsetattr( STDIN_FILENO, TCSANOW, &new_termios ) ) {
      fprintf( stderr, "Unable to write changes for stdin!\r\n" );
      exit( EXIT_FAILURE );
    }
  }

  while( ! finished ) {
    // open serial handle
    printf( "Try to open device \"%s\"\r\n", device );
    handle = serial_open( device );

    // error handling
    if ( INVALID_HANDLE == handle ) {
      // check for errors to skip
      if ( ENONET == errno || ENODEV == errno || EACCES == errno ) {
        fprintf( stderr, "\rWaiting for %s to be ready!\r", device );
        sleep( 1 );
        continue;
      }

      // handle exit
      exit( EXIT_FAILURE );
    }

    // progress output
    fprintf( stderr, "Listening on device %s\r\n", device );

    // Send loaded file from buffer
    printf( "Sending loaded file via serial device!\r\n" );

    // FIXME: Add sending of device and remove set of finished to true
    finished = true;
  }

  exit( EXIT_SUCCESS );
}
