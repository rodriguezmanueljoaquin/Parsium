#include <ast.h>
#include <stdlib.h>

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
	expression->op = SYMBOL_OP;
	expression->value = value;

	return expression;
}

Expression *newBool(bool b) {
	Expression *expression = malloc(sizeof(Expression));
	bool *value = malloc(sizeof(bool));
	*value = b;
	expression->type = BOOL_TYPE;
	expression->op = SYMBOL_OP;
	expression->value = value;

	return expression;
}

Expression *newOperation(OperationType op, Expression *exp1, Expression *exp2) {
	ValueType expType;

	switch (op) {
		case AND_OP:
		case OR_OP:
		case NOT_OP:
		case EQ_OP:
		case NE_OP:
			expType = BOOL_TYPE;
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
	statement->conditional->condition = condition;
	statement->conditional->affirmative = affirmative;
	statement->conditional->negative = negative;
	statement->type = CONDITIONAL_STMT;
	return statement;
}

Statement *newAssignment(char *symbol, Expression *value) {
	Statement *statement = malloc(sizeof(Statement));
	statement->assignment->symbol = symbol;
	statement->assignment->value = value;
	statement->type = ASSIGN_STMT;
	return statement;
}

Statement *newDeclaration(ValueType type, char *symbol, Expression *value) {
	Statement *statement = malloc(sizeof(Statement));
	statement->declaration->type = type;
	statement->declaration->symbol = symbol;
	statement->declaration->value = value;
	statement->type = DECLARE_STMT;
	return statement;
}

Statement *newLoop(); // TODO: Hacer xd

Statement *newBlock(LinkedList *list) {
	Statement *statement = malloc(sizeof(Statement));
	statement->statements = list;
	statement->type = BLOCK_STMT;
	return statement;
}
