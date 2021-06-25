CFLAGS= -std=gnu99 -pedantic -pedantic-errors -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Wno-implicit-fallthrough -g
YFLAGS= -v -d
LLFLAG= -ll

LEX_SOURCE= lexer.l
YACC_SOURCE= grammar.y
C_SOURCES= translate.c linkedlist.c ast.c lex.yy.c y.tab.c machinestate.c

all: lexgrammar  yaccgrammar compiler

fsanitize: lexgrammar yaccgrammar compiler_fsanitize

lexgrammar: $(LEX_SOURCE)
	$(LEX) $(LFLAGS) $^

yaccgrammar: $(YACC_SOURCE)
	$(YACC) $(YFLAGS) $^

compiler:
	$(CC) $(CFLAGS) -I./include $(C_SOURCES) $(LYFLAG) -o pc

compiler_fsanitize:
	$(CC) $(CFLAGS) -I./include $(C_SOURCES) $(LYFLAG) -fsanitize=address -o pc

clean:
	rm -rf pc *.o lex.yy.c y.output y.tab.c y.tab.h 

.PHONY: all fsanitize clean
