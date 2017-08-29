#ifndef ATOLLA_CONFIG_H
#define ATOLLA_CONFIG_H

#define ATOLLA_VERSION_MAJOR 1
#define ATOLLA_VERSION_MINOR 0
#define ATOLLA_VERSION_PATCH 0

#define HAVE_SIZE_T
#define HAVE_BOOL
#define HAVE_UINT8_T
#define HAVE_UINT16_T

#define HAVE_MEMSET
#define HAVE_MEMCPY
#define HAVE_MALLOC

//#cmakedefine HAVE_WINDOWS_SLEEP
//#cmakedefine HAVE_POSIX_SLEEP
#define HAVE_ARDUINO_SLEEP

//#cmakedefine HAVE_WINSOCK2
//#cmakedefine HAVE_POSIX_SOCKETS
#define HAVE_ARDUINO_WIFI_UDP

// Uncomment both to detect endianness at compile time
//#cmakedefine NATIVE_BIG_ENDIAN
//#cmakedefine NATIVE_LITTLE_ENDIAN

#endif // ATOLLA_CONFIG_H
