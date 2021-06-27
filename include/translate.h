#ifndef TRANSLATE_H
#define TRANSLATE_H

#include <ast.h>
#include <linkedlist.h>

#define NO_CHAR "NO_CHAR"

void translate(LinkedList *ast, LinkedList *machines, LinkedList *predicates);

#endif
