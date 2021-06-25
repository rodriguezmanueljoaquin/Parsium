#ifndef TRANSLATE_H
#define TRANSLATE_H

#include <ast.h>
#include <linkedlist.h>

void translate(LinkedList *ast, LinkedList *machines);

typedef struct MachineState {
	char *symbol;
	LinkedList *transitions;
	size_t transitionSize;
} MachineState;

#endif
