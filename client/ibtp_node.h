#ifndef IBPTNODE_H
#define IBPTNODE_H

#include <windows.h>
#include "utils.h" //why doesn't work in c file?
#include "ibtp_h.h"

/*
* functions to work with node header
*/
UINT16 ibtp_btype(btnode* node); //get value of btype field in header
UINT16 ibtp_nkeys(btnode* node); //get value of bkeys field in header
void ibtp_set_header(btnode* node, UINT16 btype, UINT16 nkeys); //set values of btype and nkeys fields

/*
* functions to work with node pointers list
*/
btptr ibtp_get_ptr(btnode* node, UINT16 idx); //get pointer to node in pointers list by index
void ibtp_set_ptr(btnode* node, UINT16 idx, btptr value); //set pointer to node in pointers list by index

/*
* functions to work with offset list
*/
UINT16 ibtp_offset_pos(btnode* node, UINT16 idx); //get offset of offset value
UINT16 ibtp_get_offset(btnode* node, UINT16 idx); //get offset value
void ibtp_set_offset(btnode* node, UINT16 idx, UINT16 offset); //set offset value to appropriate index

/*
* functions to work with key-value pairs
*/
UINT16 ibtp_kv_pos(btnode* node, UINT16 idx); //get key-value pair position
key* ibtp_get_key(btnode* node, UINT16 idx, boolean replicate); //get key by index
value* ibtp_get_val(btnode* node, UINT16 idx, boolean replicate); //get value by index

/*
* function to get size of the node
*/
UINT16 ibtp_nbytes(btnode* node);

/*
* function that looks up the key in node and
* returns the index of the first kid node whose range intersects the key (kid[i] <= key)
* works for both leaf and internal nodes
*/
UINT16 ibtp_node_lookup(btnode* node, key* search_key);

/*
* update leaf node(copies content to new) and add a new key to a leaf node
*/
void ibtp_leaf_insert(btnode* newest, btnode* old, UINT16 idx, key* key, value* val);

/*
* update leaf node(copies content to new) and edit the key in a leaf node
*/
void ibtp_leaf_update(btnode* newest, btnode* old, UINT16 idx, key* key, value* val);

/*
* function copies keys from an old to new node; copies multiple KVs into the position
*/
void ibtp_node_append_range(btnode* newest, btnode* old, UINT16 dstNew, UINT16 srcOld, UINT16 n);

/*
* functions copies a KV into the position
*/
void ibtp_node_append_kv(btnode* newest, UINT16 idx, UINT64 ptr, key* key, value* val);

void translate(btnode* node);

#endif