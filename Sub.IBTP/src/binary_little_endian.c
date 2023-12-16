#include "../pch.h"
#define BINARYLITTLEENDIANIMPL
#include "binary_little_endian.h"
#undef BINARYLITTLEENDIANIMPL

UINT16 binary_little_endian_uint16(BYTE* source) {
	return *(UINT16*)(source);
}

void binary_little_endian_put_uint16(BYTE* destination, UINT16 value) {
	*(UINT16*)(destination) = value;
}

UINT64 binary_little_endian_uint64(BYTE* source) {
	return *(UINT64*)(source);
}
void binary_little_endian_put_uint64(BYTE* destination, UINT64 value) {
	*(UINT64*)(destination) = value;
}