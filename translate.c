#include <ast.h>
#include <machinestate.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <translate.h>

static void printIndentation();
static void translateMachineDefinitions(LinkedList *machines);
static void translateStatement(Statement *statement);
static void translateDeclaration(Declaration *declaration);
static void translateConditional(Conditional *conditional);
static void translateAssignment(Assignment *assignment);
static void translateExpression(Expression *expression);
static void translateConstant(ValueType type, void *value);
static void translateMachineStates(Node *firstState, char *machineSymbol, LinkedList *finalStatesSymbols);
static void translateMachineStructs(Node *firstState, char *machineSymbol);
static void translateMachineParser(char *machineSymbol);
static void translateMachineExecutionFunction(LinkedList *machineStates, char *machineSymbol, char *startNodeSymbol);
static void translateBlock(Statement *block);
static void translateLoop(Loop *loop);
static void translatePredicates(LinkedList *predicates);

static size_t indentationLevel = 0;

static char *header = "#include <stdio.h>\n"
					  "#include <stdbool.h>\n"
					  "#define N(x) (sizeof(x) / sizeof((x)[0]))\n"
					  "#define ANY NULL\n\n";

void translate(LinkedList *ast, LinkedList *machines, LinkedList *predicates) {
	printf("%s", header);

	translatePredicates(predicates); // poner prototipos y luego definirlas

	translateMachineDefinitions(machines);

	Node *currentList = ast->first;
	printf("int main() {\n");
	indentationLevel++;
	while (currentList != NULL) {
		translateStatement((Statement *)currentList->value);

		currentList = currentList->next;
	}
	indentationLevel--;
	printIndentation();
	printf("}\n");
}

static void checkMachineStates(LinkedList *machineStates, MachineType *currentMachine) {
	if (!machineStatesContains(machineStates, currentMachine->initialState))
		parseError("Initial node has no transitions");

	Node *state = currentMachine->finalStates->first;
	while (state != NULL) {
		if (!machineStatesContains(machineStates, state->value))
			parseError("Final state has no transitions");

		state = state->next;
	}
}

static void translateMachineDefinitions(LinkedList *machines) {
	Node *currentNode = machines->first;
	MachineType *currentMachine;
	char *machineSymbol;
	LinkedList *machineStates;
	while (currentNode != NULL) {
		currentMachine = ((MachineType *)((Statement *)currentNode->value)->data.declaration->value->value);
		machineSymbol = ((Statement *)currentNode->value)->data.declaration->symbol;
		machineStates = getMachineStates(currentMachine->transitions->first);
		checkMachineStates(machineStates, currentMachine);

		translateMachineStructs(machineStates->first, machineSymbol);
		translateMachineStates(machineStates->first, machineSymbol, currentMachine->finalStates);
		translateMachineExecutionFunction(machineStates, machineSymbol, currentMachine->initialState);

		currentNode = currentNode->next;
	}
}

static void translateStatement(Statement *statement) {
	switch (statement->type) {
		case CONDITIONAL_STMT:
			translateConditional(statement->data.conditional);
			break;
		case LOOP_STMT:
			translateLoop(statement->data.loop);
			break;
		case ASSIGN_STMT:
			translateAssignment(statement->data.assignment);
			putchar(';');
			break;
		case DECLARE_STMT:
			translateDeclaration(statement->data.declaration);
			break;
		case EXPRESSION_STMT:
			printIndentation();
			translateExpression(statement->data.expression);
			putchar(';');
			break;
		case BREAK_STMT:
			break;
		case RETURN_STMT:
			printIndentation();
			printf("return ");
			translateExpression(statement->data.expression);
			putchar(';');
			break;
		case BLOCK_STMT:
			translateBlock(statement);
			break;
		default:
			break;
	}
	putchar('\n');
}

static void translateLoop(Loop *loop) {
	printIndentation();
	printf("for( %s ; ", loop->init);
	translateExpression(loop->condition);
	printf(" ; %s )", loop->increment);
	translateBlock(loop->block);
}

static void translateBlock(Statement *block) {
	Node *statement = block->data.statements->first;
	printIndentation();
	printf("{\n");
	indentationLevel++;
	while (statement != NULL) {
		translateStatement(statement->value);
		statement = statement->next;
	}
	indentationLevel--;
	printIndentation();
	printf("}");
}

static void translateDeclaration(Declaration *declaration) {
	printIndentation();
	switch (declaration->type) {
		case CHAR_TYPE:
		case CHAR_ARRAY_TYPE:
			printf("char ");
			break;
		case BOOL_TYPE:
			printf("bool ");
			break;
		case STRING_TYPE:
			printf("char *");
			break;
		case INTEGER_TYPE:
		case INTEGER_ARRAY_TYPE:
			printf("long ");
			break;
		case MACHINE_TYPE:
			return;
		case PREDICATE_TYPE:
			// TODO: hacer
			break;
		case TRANSITION_TYPE:
			// TODO: hacer
			break;
		case TRANSITION_ARRAY_TYPE:
			// TODO: hacer
			break;
		case STATE_ARRAY_TYPE:
			// TODO: hacer
			break;
		default:
			printf("\n error translateDeclaration() \n");
			break;
	}
	printf("%s", declaration->symbol);
	if (declaration->type == CHAR_ARRAY_TYPE)
		printf("[]");

	if (declaration->value != NULL) {
        printf(" = ");
        translateExpression(declaration->value);
    }
	printf(";\n");
}

static void translateConditional(Conditional *conditional) {
	printIndentation();
	printf("if (");
	translateExpression(conditional->condition);
	printf(")");
	translateStatement(conditional->affirmative);

	if (conditional->negative != NULL) {
		printIndentation();
		printf("else");
		translateStatement(conditional->negative);
	}
}

static void translateAssignment(Assignment *assignment) {
	printIndentation();
	printf("%s = ", assignment->symbol);
	translateExpression(assignment->expression);
}

static void translateExpression(Expression *expression) {
	switch (expression->op) {
		case AND_OP:
			translateExpression(expression->exp1);
			printf(" && ");
			translateExpression(expression->exp2);
			break;
		case OR_OP:
			translateExpression(expression->exp1);
			printf(" || ");
			translateExpression(expression->exp2);
			break;
		case NOT_OP:
			printf("!");
			translateExpression(expression->exp1);
			break;
		case EQ_OP:
			translateExpression(expression->exp1);
			printf(" == ");
			translateExpression(expression->exp2);
			break;
		case NE_OP:
			translateExpression(expression->exp1);
			printf(" != ");
			translateExpression(expression->exp2);
			break;
		case GT_OP:
			translateExpression(expression->exp1);
			printf(" > ");
			translateExpression(expression->exp2);
			break;
		case LT_OP:
			translateExpression(expression->exp1);
			printf(" < ");
			translateExpression(expression->exp2);
			break;
		case GE_OP:
			translateExpression(expression->exp1);
			printf(" >= ");
			translateExpression(expression->exp2);
			break;
		case LE_OP:
			translateExpression(expression->exp1);
			printf(" <= ");
			translateExpression(expression->exp2);
			break;
		case PARENTHESES_OP:
			printf("(");
			translateExpression(expression->exp1);
			printf(")");
			break;
		case PLUS_OP:
			if (expression->exp1 != NULL) {
				translateExpression(expression->exp1);
			}
			printf(" + ");
			translateExpression(expression->exp2);
			break;
		case MINUS_OP:
			if (expression->exp1 != NULL) {
				translateExpression(expression->exp1);
			}
			printf(" - ");
			translateExpression(expression->exp2);
			break;
		case MULT_OP:
			translateExpression(expression->exp1);
			printf(" * ");
			translateExpression(expression->exp2);
			break;
		case DIV_OP:
			translateExpression(expression->exp1);
			printf(" / ");
			translateExpression(expression->exp2);
			break;
		case MOD_OP:
			translateExpression(expression->exp1);
			printf(" %% ");
			translateExpression(expression->exp2);
			break;
		case PARSE_OP:
			printIndentation();
			printf("run_machine_%s(\"%s\")", (char *)expression->exp1->value, (char *)expression->exp2->value);
			break;
		case EXEC_OP:
			// TODO
			break;
		case SYMBOL_OP:
			printf("%s", (char *)expression->value);
			break;
		case CONST_OP:
			translateConstant(expression->type, expression->value);
			break;
		default:
			break;
	}
}

static void translateConstant(ValueType type, void *value) {
	switch (type) {
		case CHAR_TYPE:
			printf("'%c'", *(char *)value);
			break;
		case BOOL_TYPE:
			printf("%s", (*(bool *)value) ? "true" : "false");
			break;
		case INTEGER_TYPE:
			printf("%ld", *(long *)value);
			break;
		case CHAR_ARRAY_TYPE:
			putchar('{');
			Node *current = ((LinkedList *)value)->first;
			while (current != NULL) {
				printf("'%c'", *((char *)((Expression *)current->value)->value));
				current = current->next;

				if (current != NULL)
					printf(", ");
			}

			putchar('}');
		default:
			break;
	}
}

static void translateMachineStates(Node *firstState, char *machineSymbol, LinkedList *finalStatesSymbols) {
	Node *auxNode = firstState;
	MachineState *auxStateValue;
	for (size_t i = 0; auxNode != NULL; auxNode = auxNode->next, i++) {
		auxStateValue = (MachineState *)auxNode->value;
		printIndentation();
		// Estructura de nodo
		printf("static const parser_state_transition_%s ST_%s_%s[%ld] = {\n", machineSymbol, machineSymbol, auxStateValue->symbol,
			   auxStateValue->transitionSize);
		indentationLevel++;
		Node *auxTransitionNode = auxStateValue->transitions->first;
		for (size_t j = 0; auxTransitionNode != NULL; auxTransitionNode = auxTransitionNode->next, j++) {
			printIndentation();
			printf("{.when = %s, .destination = PS_%s_%s},\n", (char *)((TransitionType *)auxTransitionNode->value)->when->symbol,
				   machineSymbol, ((TransitionType *)auxTransitionNode->value)->toState);
		}
		printIndentation();
		printf("{.when = ANY, .destination = PS_%s_ERROR},\n", machineSymbol);

		indentationLevel--;

		printIndentation();
		printf("};\n\n");
	}
	// Estructura de nodo ERROR
	printIndentation();
	printf("static const parser_state_transition_%s ST_%s_ERROR[1] = {\n", machineSymbol, machineSymbol);
	printIndentation();
	printf("\t{.when = ANY, .destination = PS_%s_ERROR},\n", machineSymbol);
	printIndentation();
	printf("};\n\n");

	// ESTRUCTURA CON TODOS LOS NODOS
	printIndentation();
	printf("static const parser_state_transition_%s *states_%s[] = {", machineSymbol, machineSymbol);
	auxNode = firstState;
	for (size_t i = 0; auxNode != NULL; auxNode = auxNode->next, i++) {
		printIndentation();
		printf("ST_%s_%s, ", machineSymbol, ((MachineState *)auxNode->value)->symbol);
	}
	printIndentation();
	printf("ST_%s_ERROR};\n\n", machineSymbol);

	// ESTRUCTURA CON LA CANTIDAD DE TRANSICIONES POR NODO
	printIndentation();
	printf("static const size_t states_transitions_size_%s[] = {", machineSymbol);
	auxNode = firstState;
	for (size_t i = 0; auxNode != NULL; auxNode = auxNode->next, i++) {
		printIndentation();
		printf("N(ST_%s_%s), ", machineSymbol, ((MachineState *)auxNode->value)->symbol);
	}
	printIndentation();
	printf("N(ST_%s_ERROR)};\n\n", machineSymbol);

	printIndentation();
	printf("static const parser_state_%s final_states_%s[] = {", machineSymbol, machineSymbol);

	auxNode = finalStatesSymbols->first;
	size_t finalStatesQuantity;
	for (finalStatesQuantity = 0; auxNode != NULL; auxNode = auxNode->next, finalStatesQuantity++) {
		printIndentation();
		printf("PS_%s_%s, ", machineSymbol, (char *)auxNode->value);
	}
	printIndentation();
	printf("};\n\n");

	printIndentation();
	printf("static const size_t final_states_size_%s = %ld;\n\n", machineSymbol, finalStatesQuantity);
}

static void translateMachineStructs(Node *firstState, char *machineSymbol) {
	printIndentation();

	// ENUM DE LOS ESTADOS DE LA MAQUINA
	printf("typedef enum {\n");

	indentationLevel++;
	Node *auxStateNode = firstState;
	for (size_t i = 0; auxStateNode != NULL; auxStateNode = auxStateNode->next, i++) {
		printIndentation();
		printf("PS_%s_%s,\n", machineSymbol, ((MachineState *)auxStateNode->value)->symbol);
	}
	printIndentation();
	printf("PS_%s_ERROR,\n", machineSymbol);
	indentationLevel--;

	printIndentation();
	printf("} parser_state_%s;\n\n", machineSymbol);

	// ESTRUCTURA DE UN NODO DE LA MAQUINA
	printIndentation();
	printf("typedef struct {\n");

	indentationLevel++;
	printIndentation();
	printf("bool (*when)(char);\n");
	printIndentation();
	printf("parser_state_%s destination;\n", machineSymbol);
	indentationLevel--;

	printIndentation();
	printf("} parser_state_transition_%s;\n\n", machineSymbol);
}

static void translateMachineParser(char *machineSymbol) {
	printIndentation();
	printf("for (size_t i = 0; parse[i] != '\\0' && current_state != PS_%s_ERROR; i++) {\n", machineSymbol);

	indentationLevel++;
	printIndentation();
	printf("current_char = parse[i];\n");
	printIndentation();
	printf("for (size_t j = 0; j < states_transitions_size_%s[current_state]; j++) {\n", machineSymbol);

	indentationLevel++;
	printIndentation();
	printf("if (states_%s[current_state][j].when == ANY || states_%s[current_state][j].when(current_char)) {\n", machineSymbol,
		   machineSymbol);

	indentationLevel++;
	printIndentation();
	printf("current_state = states_%s[current_state][j].destination;\n", machineSymbol);
	printIndentation();
	printf("break;\n");
	indentationLevel--;

	printIndentation();
	printf("}\n");
	indentationLevel--;

	printIndentation();
	printf("}\n");
	indentationLevel--;

	printIndentation();
	printf("}\n\n");
}

static void translateMachineExecutionFunction(LinkedList *machineStates, char *machineSymbol, char *startNodeSymbol) {
	printIndentation();
	printf("bool run_machine_%s(char *parse) {\n", machineSymbol);

	indentationLevel++;
	printIndentation();
	// FIXME: QUE PASA SI NO HAY NODOS?
	printf("parser_state_%s current_state = PS_%s_%s;\n", machineSymbol, machineSymbol, startNodeSymbol);
	printIndentation();
	printf("char current_char;\n\n");

	translateMachineParser(machineSymbol);

	printIndentation();
	printf("bool accepted = false;\n");
	printIndentation();
	printf("if (current_state != PS_%s_ERROR) {\n", machineSymbol);

	indentationLevel++;
	printIndentation();
	printf("for (size_t i = 0; i < final_states_size_%s; i++) {\n", machineSymbol);

	indentationLevel++;
	printIndentation();
	printf("if (current_state == final_states_%s[i]) {\n", machineSymbol);

	indentationLevel++;
	printIndentation();
	printf("accepted = true; break;\n");
	indentationLevel--;

	printIndentation();
	printf("}\n");
	indentationLevel--;

	printIndentation();
	printf("}\n");
	indentationLevel--;

	printIndentation();
	printf("}\n\n");

	printIndentation();
	printf("printf(\"%%s is %%s by the machine %s\\n\", parse, accepted ? \"accepted\" : \"rejected\");\n\n", machineSymbol);
	printIndentation();
	printf("return accepted;\n");

	indentationLevel--;
	printIndentation();
	printf("}\n\n");
}

static void printIndentation() {
	for (size_t i = 0; i < indentationLevel; i++) {
		putchar('\t');
	}
}

static void translatePredicates(LinkedList *predicates) {
	Node *node = predicates->first;
	Predicate *predicate;

	node = predicates->first;
	while (node != NULL) {
		predicate = node->value;
		printf("bool %s(char %s);\n", predicate->symbol, predicate->parameter);
		node = node->next;
	}
	putchar('\n');

	node = predicates->first;
	while (node != NULL) {
		predicate = node->value;
		printf("bool %s(char %s)", predicate->symbol, predicate->parameter);

		translateBlock(predicate->block);

		printf("\n");
		node = node->next;
	}
	putchar('\n');
}
