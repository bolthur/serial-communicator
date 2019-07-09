
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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <stdbool.h>

#include "kernel.h"

const struct {
  const char* name;
  void ( *callback )( uint32_t* );
} kernel_lookup_table[] = {
  { "kernel_rpi_prepare", &kernel_rpi_prepare },
};

/**
 * @brief Internal method calling platform specific prepare
 *
 * @param machine
 * @param file_length
 */
static void kernel_perpare( const char* machine, uint32_t* file_length ) {
  char name[ 80 ];

  // build prepare function name
  strcpy( name, "kernel_" );
  strcat( name, machine );
  strcat( name, "_prepare" );

  for (
    uint32_t i = 0;
    i < ( sizeof( kernel_lookup_table ) / sizeof( kernel_lookup_table[ 0 ] ) );
    i++
  ) {
    // skip non matching callbacks or entries without valid callback
    if (
      0 != strcmp( kernel_lookup_table[ i ].name, name )
      || ! kernel_lookup_table[ i ].callback
    ) {
      continue;
    }

    // execute callback
    kernel_lookup_table[ i ].callback( file_length );
  }
}

/**
 * @brief Method for loading kernel
 *
 * @param machine
 * @param path
 * @param file_buffer
 * @param file_length
 */
void kernel_load( const char* machine, const char* path, uint8_t** file_buffer, uint32_t* file_length ) {
  // variables
  FILE *file;
  int64_t length;

  // some output
  printf( "Loading file \"%s\" for transfer\r\n", path );

  // open file
  file = fopen( path, "rb" );
  // handle error
  if ( NULL == file ) {
    fprintf( stderr, "Unable to open file %s\r\n", path );
    return;
  }

  // go to end of file
  if ( 0 != fseek( file, 0, SEEK_END ) ) {
    fprintf( stderr, "Unable to get to file end\r\n" );
    fclose( file );
    return;
  }
  // save length
  length = ftell( file );
  if ( -1L == length ) {
    fprintf( stderr, "Unable to get file length!\r\n" );
    fclose( file );
    return;
  }
  // go to start again
  if ( 0 != fseek( file, 0, SEEK_SET ) ) {
    fprintf( stderr, "Unable to set file pointer back to beginning!\r\n" );
    fclose( file );
    return;
  }

  // allocate buffer
  *file_buffer = ( uint8_t* )malloc(
    ( uint64_t )length + 1
  );
  if ( NULL == *file_buffer ) {
    fprintf( stderr, "Unable to allocate file buffer!\r\n" );
    fclose( file );
    return;
  }

  // read file into buffer
  fread( *file_buffer, ( uint64_t )length, 1, file );

  // close file
  fclose( file );

  // set length
  *file_length = ( uint32_t )length;

  // execute further prepare
  kernel_perpare( machine, file_length );
}
