
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

#if !defined( SERIAL_H )
#define SERIAL_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#if defined( WINDOWS )
  #include <windows.h>
  typedef HANDLE serial_handle_t;
  #define INVALID_HANDLE INVALID_HANDLE_VALUE
#else
  typedef int serial_handle_t;
  #define INVALID_HANDLE -1
#endif


serial_handle_t serial_open( const char* );
void serial_close( serial_handle_t );
ssize_t serial_read( serial_handle_t, void*, size_t );
ssize_t serial_write( serial_handle_t, void*, size_t );

#endif
