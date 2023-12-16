#ifndef FREELIST_H
#define FREELIST_H

#include "ibtp_h.h"


/*! @brief Functions to access the free list node. */
int flnSize(btnode node);
btptr flnNext(btnode node);
btptr flnPtr(btnode node, int idx);
void flnSetPtr(btnode node, int idx, btptr ptr);
void flnSetHeader(btnode node, int size, btptr next);
void flnSetTotal(btnode node, int total);

// number of items in the list
int Total(FreeList* fl);
// get the nth pointer
btptr flGet(FreeList* fl, int topn);

void flPush(FreeList* fl, struct dynamic_array* freed, struct dynamic_array* reuse);

// remove `popn` pointers and add some new pointers
void Update(FreeList* fl, int popn, struct dynamic_array* freed);

#endif // !FREELIST_H