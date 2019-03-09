
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

#define SEND_AMOUNT_ONCE 250

/**
 * @brief serial handle
 */
static serial_handle_t handle = 0;

/**
 * @brief file buffer
 */
static uint8_t* file_buffer = NULL;

/**
 * @brief file length
 */
static uint32_t file_length = 0;

/**
 * @brief Cleanup method on exit
 */
static void cleanup( void ) {
  // close serial handle
  serial_close( handle );

  // free file buffer if set
  if ( NULL != file_buffer ) {
    free( file_buffer );
  }
}

/**
 * @brief Method waits for breaks from serial loader
 */
static void wait_for_break( void ) {
  int32_t breaks_received;
  uint8_t buffer;
  ssize_t bytes_received;

  // output
  printf( "Listening for 3 breaks from loader in a row\r\n" );

  // amount of breaks from serial
  breaks_received = 0;

  // loop until we received 3 breaks in a row
  while ( 3 > breaks_received ) {
    // wait for 3 breaks
    bytes_received = serial_read( handle, &buffer, 1 );

    // skip when no bytes have been transmitted
    if ( 0 >= bytes_received ) {
      continue;
    }

    // handle debug break
    if ( '\x03' == buffer ) {
      breaks_received++;
      continue;
    }

    // else case we received something different
    breaks_received = 0;
    printf( "%c", buffer );
  }
}

/**
 * @brief Method waits for ok response and exits on error
 */
static void wait_for_response( void ) {
  char size_response[ 2 ];
  int32_t size_received;
  char *p;
  ssize_t read_amount;

  // wait for response
  size_received = 0;
  p = size_response;
  while ( size_received < 2 ) {
    // read from port
    read_amount = serial_read( handle, &p[ size_received ], ( size_t )( 2 - size_received ) );

    // handle error
    if ( -1 == read_amount ) {
      printf( "Error after reading state from loader!" );
      exit( EXIT_FAILURE );
    }

    // increment position
    size_received += ( int32_t )read_amount;
  }

  if (
    'O' != size_response[ 0 ]
    || 'K' != size_response[ 1 ]
  ) {
    printf( "Error received after sending size\r\n" );
    exit( EXIT_FAILURE );
  }
}

/**
 * @brief Validate received parameter
 *
 * @param argc
 * @param argv
 */
static void validate_parameter( int argc, char** argv ) {
  // handle not enough parameter
  if ( 4 != argc ) {
    // print example and usage
    printf( "Usage: %s <target> <device> <file>\r\n", argv[ 0 ] );
    printf( "Example: %s rpi /dev/ttyUSB0 kernel.img\r\n", argv[ 0 ] );

    // exit with error
    exit( EXIT_FAILURE );
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
  char *device, *file, *target;
  bool finished = false;
  ssize_t written, bytes_received, written_kernel_amount;
  uint8_t buffer;
  int32_t remaining_size;

  // initial print of name and version
  printf( "%s %s\r\n", PACKAGE_NAME, PACKAGE_VERSION );
  // validate received parameters
  validate_parameter( argc, argv );
  // register exit function
  atexit( cleanup );

  // save device and file
  target = argv[ 1 ];
  device = argv[ 2 ];
  file = argv[ 3 ];

  // load file to transfer
  kernel_load( target, file, &file_buffer, &file_length );
  // handle error
  if ( NULL == file_buffer ) {
    exit( EXIT_FAILURE );
  }

  while( ! finished ) {
    // open serial handle
    handle = serial_open( device );
    // error handling
    if ( INVALID_HANDLE == handle ) {
      // check for errors to skip
      if ( ENOENT == errno || ENODEV == errno || EACCES == errno ) {
        printf( "Waiting for \"%s\" to be ready!\r\n", device );
        sleep( 1 );
        continue;
      }

      // handle exit
      exit( EXIT_FAILURE );
    }

    // wait for breaks from device
    wait_for_break();

    // send kernel size in bytes
    printf( "Sending file size to loader!\r\n" );
    written = serial_write( handle, &file_length, 4 );

    // wait for ok response from loader
    wait_for_response();

    // send kernel
    remaining_size = ( int32_t )file_length;
    written = 0;
    while( remaining_size > 0 ) {
      // write buffer
      written_kernel_amount = serial_write(
        handle, &file_buffer[ written ],
        SEND_AMOUNT_ONCE > remaining_size
          ? ( size_t )remaining_size
          : SEND_AMOUNT_ONCE
      );

      // handle error
      if ( -1 == written_kernel_amount ) {
        printf( "Error while sending kernel to loader!" );
        exit( EXIT_FAILURE );
      }

      // decrement size and increment written
      remaining_size -= ( int32_t )written_kernel_amount;
      written += written_kernel_amount;

      // wait for ok response from loader
      wait_for_response();
    }

    // print serial device output
    while ( true ) {
      bytes_received = serial_read( handle, &buffer, 1 );

      // skip when no bytes have been transmitted
      if ( 0 >= bytes_received ) {
        continue;
      }

      // print
      printf( "%c", buffer );
    }
  }

  // exit success
  exit( EXIT_SUCCESS );
}
