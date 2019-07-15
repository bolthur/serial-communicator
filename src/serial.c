
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
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#if defined( WINDOWS )
  #include <windows.h>
#else
  #include <termios.h>
#endif

#include "serial.h"

/**
 * @brief Method to open serial connection
 *
 * @param device device to open
 * @return serial_handle_t handle of opened device or -1 on error
 */
serial_handle_t serial_open( const char* device ) {
  #if defined( WINDOWS )
    // handle to be returned
    serial_handle_t handle;
    DCB configuration;

    // open device
    handle = CreateFile(
      device,
      GENERIC_READ | GENERIC_WRITE,
      0,
      NULL,
      OPEN_EXISTING,
      FILE_FLAG_NO_BUFFERING,
      NULL
    );
    if ( INVALID_HANDLE == handle ) {
      fprintf(
        stderr, "Unable to open device %s - %lx\r\n", device, GetLastError()
      );
      return INVALID_HANDLE;
    }

    // get configuration
    if ( 0 == GetCommState( handle, &configuration ) ) {
      fprintf(
        stderr, "Unable to get configuration of device, %lx\r\n", GetLastError()
      );
      CloseHandle( handle );
      return INVALID_HANDLE;
    }

    // update configuration
    configuration.BaudRate = 115200;
    configuration.ByteSize = 8;
    configuration.Parity = NOPARITY;
    configuration.StopBits = ONESTOPBIT;

    // set configuration
    if ( 0 == SetCommState( handle, &configuration ) ) {
      fprintf(
        stderr, "Unable to set configuration of device, %lx\r\n", GetLastError()
      );
      CloseHandle( handle );
      return INVALID_HANDLE;
    }

    // prepare timeout configuration
    COMMTIMEOUTS timeout;
    timeout.ReadIntervalTimeout = 1;
    timeout.ReadTotalTimeoutMultiplier = 1;
    timeout.ReadTotalTimeoutConstant = 1;
    timeout.WriteTotalTimeoutMultiplier = 1;
    timeout.WriteTotalTimeoutConstant = 1;

    // set timeout configuration
    if ( 0 == SetCommTimeouts( handle, &timeout ) ) {
      fprintf(
        stderr, "Unable to set timeout of device, %lx\r\n", GetLastError()
      );
      CloseHandle( handle );
      return INVALID_HANDLE;
    }

    return handle;
  #else
    // handle to be returned
    serial_handle_t handle;
    struct termios option;

    // output
    printf( "Opening specified device \"%s\"\r\n", device );

    // open device
    handle = open( device, O_RDWR | O_NOCTTY | O_NDELAY );
    if ( INVALID_HANDLE == handle ) {
      fprintf(
        stderr, "Unable to open device %s - %s\r\n", device, strerror( errno )
      );
      return INVALID_HANDLE;
    }

    // check for tty
    if ( ! isatty( handle ) ) {
      fprintf( stderr, "%s is not a tty\r\n", device );
      close( handle );
      return INVALID_HANDLE;
    }

    // get current option from port
    if ( 0 > tcgetattr( handle, &option ) ) {
      fprintf( stderr, "Unable to get attributes of device\r\n" );
      close( handle );
      return INVALID_HANDLE;
    }

    // set polling
    option.c_cc[ VTIME ] = 0;
    option.c_cc[ VMIN ] = 0;

    // set mode and no input/output/line processing masks
    option.c_cflag = CS8 | CREAD | CLOCAL;
    option.c_iflag = 0;
    option.c_lflag = 0;
    option.c_oflag = 0;

    // set baud rate to 115200
    if (
      0 > cfsetispeed( &option, B115200 )
      || 0 > cfsetospeed( &option, B115200 )
    ) {
      fprintf( stderr, "Unable to set baud rate!\r\n" );
      close( handle );
      return INVALID_HANDLE;
    }

    // set new option for port
    if ( 0 > tcsetattr( handle, TCSANOW, &option ) ) {
      fprintf( stderr, "Unable to write attributes!\r\n" );
      close( handle );
      return INVALID_HANDLE;
    }

    // finally return handle
    return handle;
  #endif
}

/**
 * @brief Method to close serial connection
 *
 * @param handle handle to close
 */
void serial_close( serial_handle_t handle ) {
  // check for invalid handle
  if ( INVALID_HANDLE == handle ) {
    return;
  }

  // close serial port
  #if defined( WINDOWS )
    CloseHandle( handle );
  #else
    close( handle );
  #endif
}

/**
 * @brief Read from serial
 *
 * @param handle
 * @param buffer
 * @param count
 */
ssize_t serial_read( serial_handle_t handle, void* buffer, size_t count ) {
  #if defined( WINDOWS )
    ssize_t val = 0;
    // write to serial
    int read_val = ReadFile(
      handle, buffer, ( DWORD )count, ( LPDWORD )&val, NULL
    );
    // handle error
    if ( 0 == read_val ) {
      fprintf( stderr, "Unable to read from serial: %lx\r\n", GetLastError() );
    }
    // return value
    return val;
  #else
    return read( handle, buffer, count );
  #endif
}

/**
 * @brief Write to serial
 *
 * @param handle
 * @param buffer
 * @param count
 * @return ssize_t
 */
ssize_t serial_write( serial_handle_t handle, void* buffer, size_t count ) {
  #if defined( WINDOWS )
    ssize_t val = 0;
    // write to serial
    int write_val = WriteFile(
      handle, buffer, ( DWORD )count, ( LPDWORD )&val, NULL
    );
    // handle error
    if ( 0 == write_val ) {
      fprintf( stderr, "Unable to write to serial: %lx\r\n", GetLastError() );
    }
    // return value
    return val;
  #else
    return write( handle, buffer, count );
  #endif
}
