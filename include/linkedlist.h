#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdbool.h>
#include <ast.h>

typedef struct Node {
	void *value;
	struct Node *next;
} Node;

typedef struct LinkedList {
	Node *first, *last;
} LinkedList;

LinkedList *newList();

void addToList(LinkedList *list, void *data);

bool machineStatesContains(LinkedList *list, char *symbol);

Node *popLastNode(LinkedList *list);

#endif
