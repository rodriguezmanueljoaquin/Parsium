#ifndef MACHINE_STATE_H
#define MACHINE_STATE_H

#include <stdbool.h>
#include <linkedlist.h>
#include <stdio.h>

typedef struct MachineState {
	char *symbol;
	LinkedList *transitions;
	size_t transitionSize;
} MachineState;

LinkedList *getMachineStates(Node *firstTransition);

#endif
