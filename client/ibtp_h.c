#define IBTPHIMPL
#include "ibtp_h.h"
#include "utils.h"
#undef IBTPHIMPL

int BTREE_PAGE_SIZE = 4096; //default value of page size

const int BTYPE_OFFSET = 0; //size of type of node cell
const int NKEYS_OFFSET = 2; //size of number of keys cell
const int POINTERS_OFFSET = 4; //offset of pointers to nodes

const int KLEN_OFFSET = 0; //size of key length cell
const int VLEN_OFFSET = 2; //size of value length cell
const int KV_OFFSET = 4; //calculate like sum of key cell size and value cell size
const int BTREE_MAX_KEY_SIZE = 1000; //limitation for key size
const int BTREE_MAX_VAL_SIZE = 3000; //limitation for value size