#ifndef AST_H

#define AST_H

#include <linkedlist.h>
#include <stdbool.h>

#define NULL_STR "NULL"

#define DEFAULT_PREDICATE_ISUPPERCASE "isUpperCase"
#define DEFAULT_PREDICATE_ISLOWERCASE "isLowerCase"
#define DEFAULT_PREDICATE_ISNUMBER "isNumber"

typedef enum {
	AND_OP,
	OR_OP,
	NOT_OP,
	EQ_OP,
	NE_OP,
	PARENTHESES_OP,
	EXEC_OP,
	SYMBOL_OP,
	CONST_OP,
	PARSE_OP,
	PLUS_OP,
	MINUS_OP,
	MULT_OP,
	DIV_OP,
	MOD_OP,
	GT_OP,
	LT_OP,
	GE_OP,
	LE_OP,
	PRINT_OP,
	READ_OP
} OperationType;

typedef enum {
	CONDITIONAL_STMT,
	LOOP_STMT,
	ASSIGN_STMT,
	DECLARE_STMT,
	BREAK_STMT,
	RETURN_STMT,
	READ_STRING_STMT,
	READ_INT_STMT,
	READ_CHAR_STMT,
	READ_BOOL_STMT,
	BLOCK_STMT,
	EXPRESSION_STMT
} StatementType;

typedef enum {
	CHAR_TYPE,
	BOOL_TYPE,
	SYMBOL_TYPE,
	STRING_TYPE,
	INTEGER_TYPE,
	INTEGER_ARRAY_TYPE,
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

typedef struct MachineType {
	LinkedList *transitions;
	char *initialState;
	LinkedList *finalStates; // lista de simbolos
} MachineType;

typedef struct Conditional {
	Expression *condition;
	Statement *affirmative;
	Statement *negative;
} Conditional;

typedef struct Assignment {
	char *symbol;
	Expression *expression;
} Assignment;

typedef struct Declaration {
	char *symbol;
	ValueType type;
	Expression *value;
} Declaration;

typedef struct Loop {
	Expression *condition;
	Statement *block;
	char *init;
	char *increment;
} Loop;

typedef struct Predicate {
	char *symbol;
	char *parameter;
	Statement *block;
} Predicate;

typedef struct PredicateCall {
	char *symbol;
	char *parameter;
	char character;
} PredicateCall;

typedef struct TransitionCondition {
	Predicate *predicate;
	char character;
} TransitionCondition;

typedef struct Transition {
	char *fromState;
	char *toState;
	TransitionCondition *condition;
} Transition;

/* Statement nodes para AST */
struct Statement {
	StatementType type;
	union data {
		Conditional *conditional;
		Assignment *assignment;
		Loop *loop;
		Declaration *declaration;
		Expression *expression;
		LinkedList *statements;
	} data;
};

typedef struct Variable {
	ValueType type;
	char *symbol;
} Variable;

void parseError(char *message);

Expression *newSymbol(char *identifier);
Expression *newString(char *string);
Expression *newChar(char c);
Expression *newBool(bool b);
Expression *newInteger(long number);
Expression *newExpression(OperationType op, Expression *exp1, Expression *exp2);
Expression *newParseExpression(char *machineSymbol, Expression *input);
Expression *newMachine(LinkedList *transitions, char *initialState, LinkedList *finalStates);
Expression *newPredicateCall(char *symbol, char *parameter, char character);
Transition *newTransition(char *fromState, char *toState, TransitionCondition *when);
TransitionCondition *newTransitionCondition(char *predicate, char character);
Statement *newConditional(Expression *condition, Statement *affirmative, Statement *negative);
Statement *newAssignment(char *symbol, Expression *value);
Statement *newDeclaration(ValueType type, char *symbol, Expression *value);
Statement *newLoop(Expression *condition, Statement *block, char *init, char *increment);
Statement *newStatement(StatementType type, Expression *expression);
Statement *newBlock(LinkedList *statementList);
void newPredicate(char *symbol, char *parameter, Statement *block);

Variable *findVariableInScope(LinkedList *list, char *symbol);
Predicate *findPredicate(char *symbol);

void pushScope();
void popScope();
LinkedList *peekScope();
Variable *findVariable(char *symbol);

#endif
