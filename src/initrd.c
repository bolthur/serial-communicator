
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

/**
 * @brief Method for loading initial ramdisk
 *
 * @param machine
 * @param path
 * @param file_buffer
 * @param file_length
 */
void initrd_load( const char* path, uint8_t** file_buffer, uint32_t* file_length ) {
  // variables
  FILE *file;
  int64_t length;

  // some output
  printf( "Appending \"%s\" to kernel for transfer\r\n", path );

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
  *file_buffer = ( uint8_t* )realloc(
    *file_buffer,
    ( size_t )( *file_length + ( uint64_t )length + 1 )
  );
  if ( NULL == *file_buffer ) {
    fprintf( stderr, "Unable to allocate file buffer!\r\n" );
    fclose( file );
    return;
  }

  // read file into buffer
  fread( *file_buffer + *file_length, ( size_t )length, 1, file );

  // close file
  fclose( file );

  // set length
  *file_length += ( uint32_t )length;
}