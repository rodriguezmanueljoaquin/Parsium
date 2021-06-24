#ifndef AST_H

#define AST_H

#include <linkedlist.h>
#include <stdbool.h>

typedef enum { AND_OP, OR_OP, NOT_OP, EQ_OP, NE_OP, PARENTHESES_OP, EXEC_OP, SYMBOL_OP, CONST_OP } OperationType;

typedef enum { CONDITIONAL_STMT, LOOP_STMT, ASSIGN_STMT, DECLARE_STMT, BREAK_STMT, RETURN_STMT, BLOCK_STMT } StatementType;

typedef enum {
	CHAR_TYPE,
	BOOL_TYPE,
	SYMBOL_TYPE,
	STRING_TYPE,
	MACHINE_TYPE,
	PREDICATE_TYPE,
	TRANSITION_TYPE,
	CHAR_ARRAY_TYPE,
	TRANSITION_ARRAY_TYPE,
	STATE_ARRAY_TYPE
} ValueType;

typedef struct Statement Statement;

/* Expression node para AST */
typedef struct Expression {
	void *value;
	OperationType op;
	ValueType type;
	struct Expression *exp1, *exp2;
} Expression;

typedef struct Conditional {
	Expression *condition;
	Statement *affirmative;
	Statement *negative;
} Conditional;

typedef struct Assignment {
	char *symbol;
	Expression *value;
} Assignment;

// typedef struct Loop {
// 	// para despues, deberia recibir una coleccion, tener un statement, y una variable del elemento actual
// } Loop;

typedef struct Declaration {
	char *symbol;
	ValueType type;
	Expression *value;
} Declaration;

/* Statement nodes para AST */
struct Statement {
	StatementType type;
	union data {
		Conditional *conditional;
		Assignment *assignment;
		// Loop *loop;
		Declaration *declaration;
		LinkedList *statements;
	} data;
};

Expression *newSymbol(char *identifier);
Expression *newChar(char c);
Expression *newBool(bool b);
Expression *newOperation(OperationType op, Expression *exp1, Expression *exp2);
Statement *newConditional(Expression *condition, Statement *affirmative, Statement *negative);
Statement *newAssignment(char *symbol, Expression *value);
Statement *newDeclaration(ValueType type, char *symbol, Expression *value);
Statement *newLoop(); // TODO: Hacer xd
Statement *newBlock(LinkedList *list);

#endif
