#include <ast.h>
#include <stdlib.h>
#include <stdio.h>

Expression *newSymbol(char *identifier) {
	Expression *expression = malloc(sizeof(Expression));
	expression->type = SYMBOL_TYPE;
	expression->op = SYMBOL_OP;
	expression->value = identifier;

	return expression;
}

Expression *newChar(char c) {
	Expression *expression = malloc(sizeof(Expression));
	char *value = malloc(sizeof(char));
	*value = c;
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

TransitionType *newTransition(char *fromState, char *toState, Expression *when) {
	TransitionType *transition = malloc(sizeof(TransitionType));
	transition->fromState = fromState;
	transition->toState = toState;
	transition->when = when;

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
	ValueType expType;

	switch (op) {
		case AND_OP:
		case OR_OP:
		case NOT_OP:
		case EQ_OP:
		case NE_OP:
			expType = BOOL_TYPE;
			break;
		case PARSE_OP:
			expType = PARSE_TYPE;
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

Statement *newConditional(Expression *condition, Statement *affirmative, Statement *negative) {
	Statement *statement = malloc(sizeof(Statement));
	statement->data.conditional = malloc(sizeof(Conditional));
	statement->data.conditional->condition = condition;
	statement->data.conditional->affirmative = affirmative;
	statement->data.conditional->negative = negative;
	statement->type = CONDITIONAL_STMT;
	return statement;
}

Statement *newAssignment(char *symbol, Expression *value) {
	Statement *statement = malloc(sizeof(Statement));
	statement->data.assignment = malloc(sizeof(Assignment));
	statement->data.assignment->symbol = symbol;
	statement->data.assignment->expression = value;
	statement->type = ASSIGN_STMT;
	return statement;
}

Statement *newDeclaration(ValueType type, char *symbol, Expression *value) {
	Statement *statement = malloc(sizeof(Statement));
	statement->data.declaration = malloc(sizeof(Declaration));
	statement->data.declaration->type = type;
	statement->data.declaration->symbol = symbol;
	statement->data.declaration->value = value;
	statement->type = DECLARE_STMT;
	return statement;
}

Statement *newLoop(); // TODO: Hacer xd

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
