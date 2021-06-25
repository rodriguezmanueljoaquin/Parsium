#include <linkedlist.h>
#include <stdlib.h>
#include <string.h>
#include <translate.h>

LinkedList *newList() {
	LinkedList *new = malloc(sizeof(LinkedList));
	new->first = NULL;
	new->last = NULL;

	return new;
}

void addToList(LinkedList *list, void *data) {
	Node *new = malloc(sizeof(Node));

	new->value = data;
	new->next = NULL;

	if (list->first == NULL)
		list->first = new;

	if (list->last != NULL)
		list->last->next = new;

	list->last = new;
}

bool machineStatesContains(LinkedList *list, char *symbol) {
	Node *aux = list->first;
	while (aux != NULL) {
		if (strcmp(((MachineState *)aux->value)->symbol, symbol) == 0)
			return true;

		aux = aux->next;
	}
	return false;
}
