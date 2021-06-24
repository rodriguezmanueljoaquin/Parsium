#ifndef LINKED_LIST_H

#define LINKED_LIST_H

typedef struct Node {
	void *value;
	struct Node *next;
} Node;

typedef struct LinkedList {
	Node *first, *last;
} LinkedList;

LinkedList *newList();

void addToList(LinkedList *list, void *data);

#endif
