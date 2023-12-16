#ifndef NET_H
#define NET_H

struct _net_namespace {
    struct {
        int (*start)(WORD);
        int (*clear)();
    } lib;
};

#if NET_NAMESPACE == NET_NAMESPACE_DISABLED
#include "lib.h"
#else
extern struct _net_namespace mdlNet;
#endif // NETIMPL

#endif