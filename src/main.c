
/**
 * Copyright (C) 2019 - 2020 bolthur project.
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
#include <getopt.h>

#include "serial.h"
#include "kernel.h"
#include "initrd.h"

typedef struct {
  const char* name;
  char* data;
} argument_data_t;

#define INDEX_TARGET 0
#define INDEX_DEVICE 1
#define INDEX_KERNEL 2
#define INDEX_INITRD 3

#define SEND_AMOUNT_ONCE 250

/**
 * @brief serial handle
 */
static serial_handle_t handle = 0;

/**
 * @brief file buffer
 */
static uint8_t* kernel_buffer = NULL;

/**
 * @brief file length
 */
static uint32_t kernel_length = 0;

/**
 * @brief file buffer
 */
static uint8_t* initrd_buffer = NULL;

/**
 * @brief file length
 */
static uint32_t initrd_length = 0;

/**
 * @brief argument data
 */
static argument_data_t argument[] = {
  { "target", NULL },
  { "device", NULL },
  { "kernel", NULL },
  { "initrd", NULL }
};

/**
 * @brief Cleanup method on exit
 */
static void cleanup( void ) {
  // close serial handle
  serial_close( handle );

  // free file buffer if set
  if ( NULL != kernel_buffer ) {
    free( kernel_buffer );
  }
  if ( NULL != initrd_buffer ) {
    free( initrd_buffer );
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
 * @brief Helper to print usage
 *
 * @param name program name
 */
static void print_usage( const char* name ) {
  // print example and usage
  printf(
    "Usage: %s --target <target> --device <device> --kernel <kernel> [--initrd <initrd>]\r\n",
    name
  );
  printf(
    "Example: %s --target rpi --device /dev/ttyUSB0 --kernel kernel.img\r\n",
    name
  );
  exit( EXIT_FAILURE );
}

/**
 * @brief Validate received parameter
 *
 * @param argc
 * @param argv
 */
static void validate_parameter( int argc, char** argv ) {
  struct option long_option[] = {
    { "target", required_argument, 0, 't' },
    { "device", required_argument, 0, 'd' },
    { "kernel", required_argument, 0, 'k' },
    { "initrd", required_argument, 0, 'i' },
    { 0, 0, 0, 0 }
  };

  int opt, long_index, idx;

  while ( -1 != ( opt = getopt_long( argc, argv, "t:d:k:i:", long_option, &long_index ) ) ) {
    switch ( opt ) {
      case 't':
        idx = INDEX_TARGET;
        break;
      case 'd':
        idx = INDEX_DEVICE;
        break;
      case 'k':
        idx = INDEX_KERNEL;
        break;
      case 'i':
        idx = INDEX_INITRD;
        break;
      default:
        printf( "%c", opt );
        print_usage( argv[ 0 ] );
        // idx = -1;
    }

    // push back option
    argument[ idx ].data = optarg;
  }

  // handle missing necessary data
  if (
    NULL == argument[ INDEX_TARGET ].data
    || NULL == argument[ INDEX_DEVICE ].data
    || NULL == argument[ INDEX_KERNEL ].data
  ) {
    print_usage( argv[ 0 ] );
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
  char *device, *kernel, *target, *initrd;
  bool finished = false;
  ssize_t written, bytes_received, written_kernel_amount, written_initrd_amount;
  uint8_t buffer;
  int32_t remaining_size;
  uint32_t kernel_type, initrd_type;

  // initial print of name and version
  printf( "%s %s\r\n", PACKAGE_NAME, PACKAGE_VERSION );
  // validate received parameters
  validate_parameter( argc, argv );
  // register exit function
  atexit( cleanup );

  // save device and file
  target = argument[ INDEX_TARGET ].data;
  device = argument[ INDEX_DEVICE ].data;
  kernel = argument[ INDEX_KERNEL ].data;
  initrd = argument[ INDEX_INITRD ].data;

  // load file to transfer
  kernel_load( target, kernel, &kernel_buffer, &kernel_length, &kernel_type );
  // handle error
  if ( NULL == kernel_buffer ) {
    exit( EXIT_FAILURE );
  }

  // handle initrd
  if ( NULL != initrd ) {
    // load initrd
    initrd_load( target, initrd, &initrd_buffer, &initrd_length, &initrd_type );
    // handle error
    if ( NULL == initrd_buffer ) {
      exit( EXIT_FAILURE );
    }
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

    // send type kernel
    printf( "Sending type kernel (%d) to loader!\r\n", kernel_type );
    written = serial_write( handle, &kernel_type, 4 );

    // wait for ok response from loader
    wait_for_response();

    // send kernel size in bytes
    printf( "Sending kernel file size ( %d ) to loader!\r\n", kernel_length );
    written = serial_write( handle, &kernel_length, 4 );

    // wait for ok response from loader
    wait_for_response();

    // send kernel
    printf( "Sending kernel to loader!\r\n" );
    remaining_size = ( int32_t )kernel_length;
    written = 0;
    while( remaining_size > 0 ) {
      // write buffer
      written_kernel_amount = serial_write(
        handle, &kernel_buffer[ written ],
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

    // handle initrd
    if ( NULL != initrd ) {
      // send type initrd
      printf( "Sending type initrd ( %d ) to loader!\r\n", initrd_type );
      written = serial_write( handle, &initrd_type, 4 );

      // wait for ok response from loader
      wait_for_response();

      // send kernel size in bytes
      printf( "Sending initrd file size to loader( %d )!\r\n", initrd_length );
      written = serial_write( handle, &initrd_length, 4 );

      // wait for ok response from loader
      wait_for_response();

      // send initrd
      printf( "Sending initrd to loader!\r\n" );
      remaining_size = ( int32_t )initrd_length;
      written = 0;
      while( remaining_size > 0 ) {
        // write buffer
        written_initrd_amount = serial_write(
          handle, &initrd_buffer[ written ],
          SEND_AMOUNT_ONCE > remaining_size
            ? ( size_t )remaining_size
            : SEND_AMOUNT_ONCE
        );

        // handle error
        if ( -1 == written_initrd_amount ) {
          printf( "Error while sending kernel to loader!" );
          exit( EXIT_FAILURE );
        }

        // decrement size and increment written
        remaining_size -= ( int32_t )written_initrd_amount;
        written += written_initrd_amount;

        // wait for ok response from loader
        wait_for_response();
      }
    }

    printf( "Sending go command to start booting!\r\n" );
    // send go
    char go_command[ 2 ] = { "GO" };
    // send command
    written = serial_write( handle, go_command, 2 );
    // handle error
    if ( -1 == written ) {
      printf( "Error while sending command \"go\" loader!" );
      exit( EXIT_FAILURE );
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
