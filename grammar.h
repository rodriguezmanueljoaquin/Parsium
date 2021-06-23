#ifndef GRAMMAR_H

#define GRAMMAR_H

typedef enum {CHAR_TYPE, BOOL_TYPE, STRING_TYPE, MACHINE_TYPE, PREDICATE_TYPE, TRANSITION_TYPE} type;

typedef struct Value{
    void *valPtr;
    type valType;
} Value;

void createEmptySymbol(char *typeStr, char *identifier);

void createSymbol(char *typeStr, char *identifier, Value val);

#endif
