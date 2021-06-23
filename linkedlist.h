#ifndef LINKED_LIST_H

#define LINKED_LIST_H

typedef struct Node {
	void *value;
	Node *next;
} Node;

typedef struct LinkedList {
	Node *first, *last;
} LinkedList;

#endif
