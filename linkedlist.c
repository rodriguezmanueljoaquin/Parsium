#include <ast.h>
#include <linkedlist.h>
#include <machinestate.h>
#include <stdlib.h>
#include <string.h>

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

Node *popLastNode(LinkedList *list) {
	if(list == NULL || list->first == NULL || list->last == NULL)
		return NULL;

	Node *aux = list->first, *last = list->last;
	if(aux->next == NULL) {
		list->first = NULL;
		list->last = NULL;
		return last;
	}
	
	while(aux->next->next != NULL)
		aux = aux->next;

	list->last = aux;
	aux->next = NULL;
	return last;
}
