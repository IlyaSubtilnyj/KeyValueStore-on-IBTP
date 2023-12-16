#ifndef IBTPTREE_H
#define IBTPTREE_H

#include "ibtp_h.h"

value* ibtp_tree_get(btree* tree, btnode* node, key* key);
btnode* ibtp_tree_insert(btree* tree, btnode* node, key* key, value* val);
void ibtp_node_insert(btree* tree, btnode* new, btnode* old, UINT16 idx, key* key, value* val);
void ibtp_node_split_2(btnode* left, btnode* right, btnode* old);
int ibtp_node_split_3(btnode* old, struct mem_dump splited[]);
void ibtp_node_replace_kid_n(btree* tree, btnode* new, btnode* old, UINT16 idx, btnode kids[], int kids_count);
void ibtp_node_replace_2_kid(btnode* new, btnode* old, UINT16 idx, btptr merged_child, key* first_merged_child_key);
void ibtp_leaf_delete(btnode* new, btnode* old, UINT16 idx);
btnode* ibtp_tree_delete(btree* tree, btnode node, key* key);
btnode* ibtp_node_delete(btree* tree, btnode* node, UINT16 idx, key* key);
void ibtp_node_merge(btnode* new, btnode* left, btnode* right);
btnode ibtp_should_merge(btree* tree, btnode* node, UINT16 idx, btnode* updated, int* status);
boolean Delete(btree* tree, key* key);
void Insert(btree* tree, key* key, value* val);
value* TreeGet(btree* tree, key* key);


#endif // !IBTPTREE_H