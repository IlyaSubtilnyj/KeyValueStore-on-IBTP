#include "../pch.h"
#define UTILSIMPL
#include "utils.h"
#undef UTILSIMPL

struct _utils_namespace mdlUtils = {
    .bytes = {
        .compare = NULL
    },
    .binary = {
        .little_endian = {
            .uint16 = binary_little_endian_uint16,
            .put_uint16 = binary_little_endian_put_uint16,
            .uint64 = binary_little_endian_uint64,
            .put_uint64 = binary_little_endian_put_uint64
        }
    }
};