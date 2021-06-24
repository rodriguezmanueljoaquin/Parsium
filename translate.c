#include <ast.h>
#include <stdio.h>
#include <stdlib.h>
#include <translate.h>

static void translateStatement(Statement *statement);
static void translateDeclaration(Declaration *declaration);
static void translateConditional(Conditional *conditional);
static void translateAssignment(Assignment *assignment);
static void translateExpression(Expression *expression);
static void translateConstant(ValueType type, void *value);

static size_t identation_level = 0;

static void printIdentation() {
	for (size_t i = 0; i < identation_level; i++) {
		putchar('\t');
	}
}

void translate(LinkedList *list) {
	Node *current = list->first;
	printf("int main() {\n");
	identation_level++;
	while (current != NULL) {
		fflush(stdout);
		translateStatement((Statement *)current->value);
		current = current->next;
	}
	identation_level--;
	printf("}\n");
}

static void translateStatement(Statement *statement) {
	switch (statement->type) {
		case CONDITIONAL_STMT:
			translateConditional(statement->data.conditional);
			break;
		case LOOP_STMT:
			// TODO: Hacer
			break;
		case ASSIGN_STMT:
			translateAssignment(statement->data.assignment);
			putchar(';');
			break;
		case DECLARE_STMT:
			translateDeclaration(statement->data.declaration);
			putchar(';');
			break;
		case BREAK_STMT:
		case RETURN_STMT:
			break;
		case BLOCK_STMT:
		default:
			break;
	}
	putchar('\n');
}

static void translateDeclaration(Declaration *declaration) {
	printIdentation();
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
		case MACHINE_TYPE:
			// TODO: hacer
			break;
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
}

static void translateConditional(Conditional *conditional) {
	printIdentation();
	printf("if (");
	identation_level++;
	translateExpression(conditional->condition);
	identation_level--;
	printf(") {\n");
	identation_level++;
	translateStatement(conditional->affirmative);
	identation_level--;

	if (conditional->negative != NULL) {
		printf("} else {\n");
		identation_level++;
		translateStatement(conditional->negative);
		identation_level--;
	}

	printIdentation();
	printf("}");
}

static void translateAssignment(Assignment *assignment) {
	printIdentation();
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
		case PARENTHESES_OP:
			printf("(");
			translateExpression(expression->exp1);
			printf(")");
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
			printf("%s", *(bool *)value ? "true" : "false");
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
