#include <ast.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

LinkedList *variableScopes;
LinkedList *predicates;

void parseError(char *message);
static void checkTypeWithExit(ValueType type, Expression *value);
static bool checkType(ValueType type, Expression *value);

void parseError(char *message) {
	fprintf(stderr, "Syntax error: %s\n", message);
	exit(-1);
}

Expression *newSymbol(char *identifier) {
	if (findVariable(identifier) == NULL)
		parseError("Variable undefined");
	Expression *expression = malloc(sizeof(Expression));
	expression->type = SYMBOL_TYPE;
	expression->op = SYMBOL_OP;
	expression->value = identifier;

	return expression;
}

Expression *newInteger(long number) {
	long *value = malloc(sizeof(long));
	*value = number;
	Expression *expression = malloc(sizeof(Expression));
	expression->type = INTEGER_TYPE;
	expression->op = CONST_OP;
	expression->value = value;

	return expression;
}

Expression *newString(char *string) {
	Expression *expression = malloc(sizeof(Expression));
	expression->type = SYMBOL_TYPE;
	expression->op = SYMBOL_OP;
	expression->value = string;

	return expression;
}

Expression *newChar(char c) {
	char *value = malloc(sizeof(char));
	*value = c;
	Expression *expression = malloc(sizeof(Expression));
	expression->type = CHAR_TYPE;
	expression->op = CONST_OP;
	expression->value = value;

	return expression;
}

Expression *newBool(bool b) {
	Expression *expression = malloc(sizeof(Expression));
	bool *value = malloc(sizeof(bool));
	*value = b;
	expression->type = BOOL_TYPE;
	expression->op = CONST_OP;
	expression->value = value;

	return expression;
}

Expression *newArray(LinkedList *list, ValueType type) {
	Expression *expression = malloc(sizeof(Expression));
	expression->type = type;
	expression->op = CONST_OP;
	expression->value = list;

	return expression;
}

TransitionType *newTransition(char *fromState, char *toState, char *when) {
	Predicate *predicate = findPredicate(when);
	if (predicate == NULL)
		parseError("Predicate undefined");

	TransitionType *transition = malloc(sizeof(TransitionType));
	transition->fromState = fromState;
	transition->toState = toState;
	transition->when = predicate;

	return transition;
}

Expression *newMachine(LinkedList *transitions, char *initialState, LinkedList *finalStates) {
	Expression *expression = malloc(sizeof(Expression));
	expression->type = MACHINE_TYPE;
	expression->op = CONST_OP;
	expression->value = malloc(sizeof(MachineType));
	((MachineType *)expression->value)->transitions = transitions;
	((MachineType *)expression->value)->initialState = initialState;
	((MachineType *)expression->value)->finalStates = finalStates;
	expression->exp1 = NULL;
	expression->exp2 = NULL;

	return expression;
}

Expression *newExpression(OperationType op, Expression *exp1, Expression *exp2) {
	ValueType expType = 0;

	switch (op) {
		case AND_OP:
		case OR_OP:
			checkTypeWithExit(BOOL_TYPE, exp2);
		case NOT_OP:
			checkTypeWithExit(BOOL_TYPE, exp1);
		case PARSE_OP:
		case EQ_OP:
		case NE_OP:
			expType = BOOL_TYPE;
			break;
		case GT_OP:
		case LT_OP:
		case GE_OP:
		case LE_OP:
			if ((checkType(INTEGER_TYPE, exp1) && checkType(INTEGER_TYPE, exp2)) ||
				(checkType(CHAR_TYPE, exp1) && checkType(CHAR_TYPE, exp2))) {
				expType = BOOL_TYPE;
				break;
			} else
				parseError("Error: Only comparable types are char and int");
		case MULT_OP:
		case DIV_OP:
		case MOD_OP:
		case PLUS_OP:
		case MINUS_OP:
			if (exp1 != NULL)
				checkTypeWithExit(INTEGER_TYPE, exp1);
			checkTypeWithExit(INTEGER_TYPE, exp2);
			expType = INTEGER_TYPE;
			break;
		case PARENTHESES_OP:
			// HAY QUE CHECKEAR TIPO SIN MATAR AL COMPILADOR
			if (checkType(INTEGER_TYPE, exp1))
				expType = INTEGER_TYPE;
			else if (checkType(BOOL_TYPE, exp1))
				expType = BOOL_TYPE;
			else
				parseError("Parentheses operator may only be used with integer and boolean values");
			break;
		case EXEC_OP:
		case SYMBOL_OP:
			return NULL;
		default:
			expType = exp1->type;
			break;
	}

	Expression *expression = malloc(sizeof(Expression));
	expression->type = expType;
	expression->op = op;
	expression->exp1 = exp1;
	expression->exp2 = exp2;

	return expression;
}

Expression *newParseExpression(char *machineSymbol, char *string) {
	Variable *var = findVariable(machineSymbol);
	if (var == NULL)
		parseError("Machine undefined");
	if (var->type != MACHINE_TYPE)
		parseError("Variable type conflict in parse");

	Expression *expression = malloc(sizeof(Expression));
	expression->type = BOOL_TYPE;
	expression->op = PARSE_OP;
	expression->exp1 = newSymbol(machineSymbol);
	expression->exp2 = newString(string);

	return expression;
}

Statement *newConditional(Expression *condition, Statement *affirmative, Statement *negative) {
	checkTypeWithExit(BOOL_TYPE, condition);

	Statement *statement = malloc(sizeof(Statement));
	statement->data.conditional = malloc(sizeof(Conditional));
	statement->data.conditional->condition = condition;
	statement->data.conditional->affirmative = affirmative;
	statement->data.conditional->negative = negative;
	statement->type = CONDITIONAL_STMT;
	return statement;
}

static void checkTypeWithExit(ValueType type, Expression *value) {
	if (!checkType(type, value))
		parseError("Assignment conflict");
}

static bool checkType(ValueType type, Expression *value) {
	if (value != NULL && type != value->type) {
		if (value->type != SYMBOL_TYPE || type != findVariable(value->value)->type)
			return false;
	}

	return true;
}

Statement *newAssignment(char *symbol, Expression *value) {
	Variable *var = findVariable(symbol);
	if (var == NULL)
		parseError("Variable undefined");

	checkTypeWithExit(var->type, value);

	Statement *statement = malloc(sizeof(Statement));
	statement->data.assignment = malloc(sizeof(Assignment));
	statement->data.assignment->symbol = symbol;
	statement->data.assignment->expression = value;
	statement->type = ASSIGN_STMT;
	return statement;
}

Statement *newDeclaration(ValueType type, char *symbol, Expression *value) {
	if (findVariableInScope(peekScope(), symbol) != NULL)
		parseError("Variable already defined");

	checkTypeWithExit(type, value);

	Statement *statement = malloc(sizeof(Statement));
	statement->data.declaration = malloc(sizeof(Declaration));
	statement->data.declaration->type = type;
	statement->data.declaration->symbol = symbol;
	statement->data.declaration->value = value;
	statement->type = DECLARE_STMT;

	Variable *var = malloc(sizeof(Variable));
	var->symbol = symbol;
	var->type = type;

	addToList(peekScope(), var);
	return statement;
}

Statement *newLoop(Expression *condition, Statement *block, char *init, char *increment) {
	Statement *statement = malloc(sizeof(Statement));
	statement->type = LOOP_STMT;
	statement->data.loop = malloc(sizeof(Loop));
	statement->data.loop->condition = condition;
	statement->data.loop->block = block;
	statement->data.loop->init = init;
	statement->data.loop->increment = increment;
	return statement;
}

Statement *newStatement(StatementType type, Expression *expression) {
	Statement *statement = malloc(sizeof(Statement));
	statement->type = type;
	statement->data.expression = expression;
	return statement;
}

Statement *newBlock(LinkedList *statementList) {
	Statement *statement = malloc(sizeof(Statement));
	statement->type = BLOCK_STMT;
	statement->data.statements = statementList;
	return statement;
}

void newPredicate(char *symbol, char *parameter, Statement *block) {
	if (findPredicate(symbol) != NULL)
		parseError("Predicate already defined");

	Predicate *predicate = malloc(sizeof(Predicate));
	predicate->symbol = symbol;
	predicate->block = block;
	predicate->parameter = parameter;

	addToList(predicates, predicate);
}

void pushScope() {
	LinkedList *newScope = newList();
	addToList(variableScopes, newScope);
}

void popScope() { popLastNode(variableScopes); }

LinkedList *peekScope() { return variableScopes->last->value; }

Variable *findVariable(char *symbol) {
	Node *scopeNode = variableScopes->first;
	Variable *aux = NULL;
	while (scopeNode != NULL && aux == NULL) {
		aux = findVariableInScope(scopeNode->value, symbol);
		scopeNode = scopeNode->next;
	}
	return aux;
}

Variable *findVariableInScope(LinkedList *list, char *symbol) {
	Node *node = list->first;
	while (node != NULL) {
		if (strcmp(((Variable *)node->value)->symbol, symbol) == 0)
			return node->value;

		node = node->next;
	}
	return NULL;
}

Predicate *findPredicate(char *symbol) {
	Node *aux = predicates->first;
	while (aux != NULL) {
		if (strcmp(((Predicate *)aux->value)->symbol, symbol) == 0)
			return aux->value;

		aux = aux->next;
	}
	return NULL;
}
