#ifndef BINARY_LITTLE_ENDIAN_H
#define BINARY_LITTLE_ENDIAN_H

#include <windows.h>
#include "vconfig.h"

UINT16 binary_little_endian_uint16(BYTE*);
void binary_little_endian_put_uint16(BYTE*, UINT16);
UINT64 binary_little_endian_uint64(BYTE*);
void binary_little_endian_put_uint64(BYTE*, UINT64);

#endif