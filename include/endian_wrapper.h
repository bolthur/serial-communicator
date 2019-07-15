
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

#if !defined( ENDIAN_WRAPPER_H )
#define ENDIAN_WRAPPER_H
  #if defined( OSX )
    #include <libkern/OSByteOrder.h>

    #define htobe16(x) OSSwapHostToBigInt16(x)
    #define htole16(x) OSSwapHostToLittleInt16(x)
    #define be16toh(x) OSSwapBigToHostInt16(x)
    #define le16toh(x) OSSwapLittleToHostInt16(x)

    #define htobe32(x) OSSwapHostToBigInt32(x)
    #define htole32(x) OSSwapHostToLittleInt32(x)
    #define be32toh(x) OSSwapBigToHostInt32(x)
    #define le32toh(x) OSSwapLittleToHostInt32(x)

    #define htobe64(x) OSSwapHostToBigInt64(x)
    #define htole64(x) OSSwapHostToLittleInt64(x)
    #define be64toh(x) OSSwapBigToHostInt64(x)
    #define le64toh(x) OSSwapLittleToHostInt64(x)
  #elif defined( WINDOWS )
    #include <windows.h>

    #define htobe16(x) __builtin_bswap16(x)
    #define htole16(x) (x)
    #define be16toh(x) __builtin_bswap16(x)
    #define le16toh(x) (x)

    #define htobe32(x) __builtin_bswap32(x)
    #define htole32(x) (x)
    #define be32toh(x) __builtin_bswap32(x)
    #define le32toh(x) (x)

    #define htobe64(x) __builtin_bswap64(x)
    #define htole64(x) (x)
    #define be64toh(x) __builtin_bswap64(x)
    #define le64toh(x) (x)
  #else
    #define __USE_MISC
    #include <endian.h>
  #endif
#endif

