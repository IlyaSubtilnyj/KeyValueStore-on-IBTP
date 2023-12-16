#ifndef UTILS_H
#define UTILS_H

#include <windows.h>

struct _utils_namespace {
    struct
    {
        void (*compare)();
    } bytes;
    struct {
        struct {
            UINT16 (*uint16)(BYTE*);
            void(*put_uint16)(BYTE*, UINT16);
            UINT64(*uint64)(BYTE*);
            void(*put_uint64)(BYTE*, UINT64);
        } little_endian;
    } binary;
};

#if UTILS_NAMESPACE == UTILS_NAMESPACE_DISABLED
#include "binary_little_endian.h"
    #if defined IBTPNODEIMPL || defined IBTPIMPL || defined IBTPHIMPL || defined FREELISTIMPL || defined BTREEREALIZATIONFILEMAPPINGIMPL || defined IBTPTREEIMPL
    extern struct _utils_namespace mdlUtils;
    #endif
#else
extern struct _utils_namespace mdlUtils;
#endif // NETIMPL


#endif