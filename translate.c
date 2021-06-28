#include <ast.h>
#include <machinestate.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <translate.h>

static void initGlobalStrings(LinkedList *globalVariables);
static void printIndentation();
static void printRead();
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
static void translateStringAssignment(char *symbol, Expression *expression);
static void translateStringDefinition(char *symbol, Expression *expression);
static void translateBlock(Statement *block);
static void translateLoop(Loop *loop);
static void translatePredicates(LinkedList *predicates);
static void translatePrintType(Expression *expression);
static void translateGlobalVariables(LinkedList *variables);
static void translateDefaultPredicates();
static void translateCall(PredicateCall *call);
static void print_char(char c);

static size_t indentationLevel = 0;
static bool isInitializing = true;

static char *header = "#include <stdio.h>\n"
					  "#include <stdbool.h>\n"
					  "#include <stdlib.h>\n"
					  "#include <string.h>\n"
					  "#include <unistd.h>\n"
					  "#define READ_MAX_LEN 8095\n"
					  "#define STDIN_FILENO 0\n"
					  "#define N(x) (sizeof(x) / sizeof((x)[0]))\n"
					  "#define ANY NULL\n"
					  "#define " NO_CHAR " 0\n\n"
					  "static char *read_tmp_00 = NULL;\n"
					  "static int read_tmp_01 = 0;\n"
					  "static int read_tmp_02 = 0;\n";

void translate(LinkedList *ast, LinkedList *machines, LinkedList *predicates, LinkedList *globalVariables) {
	printf("%s", header);

	translateGlobalVariables(globalVariables);
	translateDefaultPredicates();
	translatePredicates(predicates);
	translateMachineDefinitions(machines);

	Node *currentList = ast->first;
	printf("int main() {\n");
	indentationLevel++;
	initGlobalStrings(globalVariables);
	isInitializing = false;
	while (currentList != NULL) {
		translateStatement((Statement *)currentList->value);

		currentList = currentList->next;
	}
	indentationLevel--;
	printIndentation();
	printf("}\n");
}

static void translateGlobalVariables(LinkedList *variables) {
	Node *aux = variables->first;
	while (aux != NULL) {
		translateDeclaration(aux->value);
		aux = aux->next;
	}
	putchar('\n');
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
	if (declaration->value != NULL && declaration->value->op == READ_OP)
		printRead();
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
			break;
		case TRANSITION_TYPE:
			break;
		case TRANSITION_ARRAY_TYPE:
			break;
		case STATE_ARRAY_TYPE:
			break;
		default:
			printf("\n error translateDeclaration() \n");
			break;
	}
	printf("%s", declaration->symbol);
	if (declaration->type == CHAR_ARRAY_TYPE)
		printf("[]");

	if (!isInitializing && declaration->type == STRING_TYPE && declaration->value == NULL) {
		printf(" = calloc(1, sizeof(char));\n");
		return;
	}

	if (declaration->value != NULL) {
		printf(" = ");
		if (declaration->type == STRING_TYPE) {
			translateStringDefinition(declaration->symbol, (Expression *)declaration->value);
			return;
		}
		translateExpression(declaration->value);
	}
	printf(";\n");
}

static void printRead() {
	printf("read_tmp_00 = calloc(READ_MAX_LEN + 1, sizeof(char));\n"
		   "read_tmp_01 = read(STDIN_FILENO, read_tmp_00, READ_MAX_LEN);\n"
		   "if (read_tmp_01 == READ_MAX_LEN) {\n"
		   "read_tmp_02 = (unsigned char)read_tmp_00[read_tmp_01 - 1];\n"
		   "while (read_tmp_02 != '\\n' && read_tmp_02 != EOF)\n"
		   "read_tmp_02 = getchar();\n"
		   "}\n"
		   "read_tmp_00[read_tmp_01 - 1] = 0;\n");
}

static void translateStringAssignment(char *symbol, Expression *expression) {
	printf("free(%s);\n", symbol);

	printIndentation();
	if (expression->op == READ_OP) {
		printRead();
		printf("%s = read_tmp_00", symbol);
	} else {
		printf("%s = strdup(", symbol);
		translateExpression(expression);
		printf(")");
	}
}

static void translateStringDefinition(char *symbol, Expression *expression) {
	if (expression->op == READ_OP) {
		printf(" read_tmp_00");
	} else {
		printf(" strdup(");
		translateExpression(expression);
	}
	printf(");");
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
	if (assignment->expression->type == STRING_TYPE) {
		translateStringAssignment(assignment->symbol, assignment->expression);
		return;
	}

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
			printf("run_machine_%s", (char *)expression->exp1->value);
			if (expression->exp2->type == STRING_TYPE)
				printf("(\"%s\")", (char *)expression->exp2->value);
			else
				printf("(%s)", (char *)expression->exp2->value);
			break;
		case EXEC_OP:
			translateCall(expression->value);
			break;
		case SYMBOL_OP:
			printf("%s", (char *)expression->value);
			break;
		case CONST_OP:
			translateConstant(expression->type, expression->value);
			break;
		case PRINT_OP:
			printIndentation();
			printf("printf(");
			translatePrintType(expression->exp1);
			printf(");\n");
			printf("fflush(stdout);");
			break;
		default:
			break;
	}
}

static void print_char(char c) {
	switch (c) {
		case '\n':
			printf("'\\n'");
			break;
		case '\r':
			printf("'\\r'");
			break;
		case '\t':
			printf("'\\t'");
			break;
		default:
			printf("'%c'", c);
			break;
	}
}

static void translateConstant(ValueType type, void *value) {
	switch (type) {
		case CHAR_TYPE:
			print_char(*(char *)value);
			// printf("'%c'", *(char *)value);
			break;
		case BOOL_TYPE:
			printf("%s", (*(bool *)value) ? "true" : "false");
			break;
		case INTEGER_TYPE:
			printf("%ld", *(long *)value);
			break;
		case STRING_TYPE:
			printf("\"%s\"", (char *)value);
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
		Transition *auxTransition;
		char *when, *character = malloc(strlen(NO_CHAR) + 1);
		character[7] = 0;
		for (size_t j = 0; auxTransitionNode != NULL; auxTransitionNode = auxTransitionNode->next, j++) {
			auxTransition = auxTransitionNode->value;
			when = NULL_STR;
			strcpy(character, NO_CHAR);
			if (auxTransition->condition->character != 0) {
				switch (auxTransition->condition->character) {
					case '\n':
						sprintf(character, "'\\n'");
						break;
					case '\r':
						sprintf(character, "'\\r'");
						break;
					case '\t':
						sprintf(character, "'\\t'");
						break;
					default:
						sprintf(character, "'%c'", auxTransition->condition->character);
						break;
				}
			} else if (auxTransition->condition->predicate != NULL)
				when = auxTransition->condition->predicate->symbol;
			else
				when = "ANY";

			printIndentation();
			printf("{.when = %s, .destination = PS_%s_%s, .character = %s},\n", when, machineSymbol, auxTransition->toState,
				   character);
		}
		printIndentation();
		printf("{.when = ANY, .destination = PS_%s_ERROR, .character = NO_CHAR},\n", machineSymbol);

		indentationLevel--;

		printIndentation();
		printf("};\n\n");
	}

	// ESTRUCTURA CON TODOS LOS NODOS
	printIndentation();
	printf("static const parser_state_transition_%s *states_%s[] = {", machineSymbol, machineSymbol);
	auxNode = firstState;
	for (size_t i = 0; auxNode != NULL; auxNode = auxNode->next, i++) {
		printIndentation();
		printf("ST_%s_%s, ", machineSymbol, ((MachineState *)auxNode->value)->symbol);
	}
	printIndentation();
	printf("};\n\n");

	// ESTRUCTURA CON LA CANTIDAD DE TRANSICIONES POR NODO
	printIndentation();
	printf("static const size_t states_transitions_size_%s[] = {", machineSymbol);
	auxNode = firstState;
	for (size_t i = 0; auxNode != NULL; auxNode = auxNode->next, i++) {
		printIndentation();
		printf("N(ST_%s_%s), ", machineSymbol, ((MachineState *)auxNode->value)->symbol);
	}
	printIndentation();
	printf("};\n\n");

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
	printf("char character;\n");
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
	printf("if (states_%s[current_state][j].character != NO_CHAR) {\n", machineSymbol);

	indentationLevel++;
	printIndentation();
	printf("if (current_char == states_%s[current_state][j].character) {\n", machineSymbol);
	indentationLevel++;
	printIndentation();
	printf("current_state = states_%s[current_state][j].destination;\n", machineSymbol);
	printIndentation();
	printf("break;}\n");
	indentationLevel--;
	indentationLevel--;

	printIndentation();
	printf("} else if (states_%s[current_state][j].when == ANY || states_%s[current_state][j].when(current_char)){\n",
		   machineSymbol, machineSymbol);
	indentationLevel++;
	printIndentation();
	printf("current_state = states_%s[current_state][j].destination;\n", machineSymbol);
	printIndentation();
	printf("break;}\n");
	indentationLevel--;
	indentationLevel--;

	printIndentation();
	printf("}\n\n");
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

static void translatePrintType(Expression *expression) {
	ValueType printType;
	if (expression->type == SYMBOL_TYPE)
		printType = findVariable((char *)expression->value)->type;
	else
		printType = expression->type;

	switch (printType) {
		case CHAR_TYPE:
			printf("\"%%c\", ");
			translateExpression(expression);
			break;
		case INTEGER_TYPE:
			printf("\"%%ld\", ");
			translateExpression(expression);
			break;
		case STRING_TYPE:
			printf("\"%%s\", ");
			translateExpression(expression);
			break;
		case BOOL_TYPE:
			printf("\"%%s\", ");
			translateExpression(expression);
			printf(" ? \"true\" : \"false\"");
			break;
		case SYMBOL_TYPE:
			printf("elbeto");
			break;
		default:
			break;
	}
}

static void translateDefaultPredicates() {
	printf("bool %s(char x) {\n"
		   "\tif ('A' <= x && x <= 'Z')\n"
		   "\t\treturn true;\n"
		   "\treturn false;\n"
		   "}\n\n",
		   DEFAULT_PREDICATE_ISUPPERCASE);

	printf("bool %s(char x) {\n"
		   "\tif ('a' <= x && x <= 'z')\n"
		   "\t\treturn true;\n"
		   "\treturn false;\n"
		   "}\n\n",
		   DEFAULT_PREDICATE_ISLOWERCASE);

	printf("bool %s(char x) {\n"
		   "\tif ('0' <= x && x <= '9')\n"
		   "\t\treturn true;\n"
		   "\treturn false;\n"
		   "}\n\n",
		   DEFAULT_PREDICATE_ISNUMBER);
}

static void translateCall(PredicateCall *call) {
	if (call->parameter != NULL)
		printf("%s(%s)", call->symbol, call->parameter);
	else
		printf("%s('%c')", call->symbol, call->character);
}

static void initGlobalStrings(LinkedList *globalVariables) {
	Node *aux = globalVariables->first;
	while (aux != NULL) {
		Declaration *dec = (Declaration *)aux->value;
		if (dec->type == STRING_TYPE) {
			printIndentation();
			printf("%s = calloc(1, sizeof(char));\n", dec->symbol);
		}
		aux = aux->next;
	}
}
