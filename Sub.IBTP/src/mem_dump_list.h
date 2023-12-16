#pragma once
#include "mem_dump.h"

struct mem_dump_node {
	struct mem_dump data;
	struct chunk_node* next;
};
void add_mem_dump_node(struct mem_dump_node** head, struct mem_dump data);
struct mem_dump get_mem_dump_node(struct mem_dump_node* head, int index);
int count_mem_dump_nodes(struct mem_dump_node* head);
void delete_mem_dump_list(struct mme_dump_node** head);
void delete_mem_dump_node(struct mme_dump_node** head, int value);

void add_mem_dump_node(struct mem_dump_node** head, struct mem_dump data) {
	struct mem_dump_node* new_node = (struct mem_dump_node*)malloc(sizeof(struct mem_dump_node));
	new_node->data = data;
	new_node->next = NULL;
	if (*head == NULL) {
		*head = new_node;
	}
	else {
		struct mem_dump_node* current = *head;
		while (current->next != NULL) {
			current = current->next;
		}
		current->next = new_node;
	}
}

struct mem_dump get_mem_dump_node(struct mem_dump_node* head, int index) {
	struct mem_dump_node* current = head;
	int count = 0;
	while (current != NULL) {
		if (count == index) {
			return current->data;
		}
		count++;
		current = current->next;
	}
	return *dump_dummy(); // index out of range
}

int count_mem_dump_nodes(struct mem_dump_node* head) {
	int count = 0;
	struct mem_dump_node* current = head;

	while (current != NULL) {
		count++;
		current = current->next;
	}

	return count;
}

void delete_mem_dump_node(struct mem_dump_node** head, struct mem_dump value) {
	struct mem_dump_node* current = *head;
	struct mem_dump_node* previous = NULL;

	while (current != NULL) {
		if (current->data.dump == value.dump) {
			if (previous == NULL) {
				*head = current->next;
			}
			else {
				previous->next = current->next;
			}
			free(current);
			return;
		}
		previous = current;
		current = current->next;
	}
}

void delete_mem_dump_list(struct mem_dump_node** head) {
	struct mem_dump_node* current = *head;
	struct mem_dump_node* next;
	while (current != NULL) {
		next = current->next;
		free(current);
		current = next;
	}
	*head = NULL;
}