#define NETIMPL
#include "net.h"
#undef NETIMPL

#include "net_h.h"

struct _net_namespace mdlNet = {
    .lib = {
        .start = lib_start,
        .clear = lib_clear
    }
};