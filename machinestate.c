#include <ast.h>
#include <machinestate.h>
#include <stdlib.h>
#include <string.h>

static MachineState *newMachineState(char *symbol) {
	MachineState *newState = malloc(sizeof(MachineState));
	newState->symbol = symbol;
	newState->transitions = newList();
	newState->transitionSize = 1; // Todos los estados tienen como ultima transicion una hacia error

	return newState;
}

static void addTransitionToMachineState(LinkedList *machineStates, char *fromState, TransitionType *transition) {
	Node *aux = machineStates->first;
	while (aux != NULL) {
		if (strcmp(((MachineState *)aux->value)->symbol, fromState) == 0) {
			addToList(((MachineState *)aux->value)->transitions, transition);
			((MachineState *)aux->value)->transitionSize++;
		}

		aux = aux->next;
	}
}

LinkedList *getMachineStates(Node *firstTransition) {
	LinkedList *currentStates = newList();
	Node *auxNode = firstTransition;
	for (size_t i = 0; auxNode != NULL; auxNode = auxNode->next, i++) {
		TransitionType *currentTransition = (TransitionType *)auxNode->value;
		if (!machineStatesContains(currentStates, currentTransition->fromState)) {
			addToList(currentStates, newMachineState(currentTransition->fromState));
			addTransitionToMachineState(currentStates, currentTransition->fromState, currentTransition);
		} else {
			addTransitionToMachineState(currentStates, currentTransition->fromState, currentTransition);
		}
		if (!machineStatesContains(currentStates, currentTransition->toState))
			addToList(currentStates, newMachineState(currentTransition->toState));
	}

	return currentStates;
}
