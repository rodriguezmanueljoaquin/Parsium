CFLAGS= -std=gnu99 -pedantic -pedantic-errors -Wall -Wextra  -Wno-unused-parameter -Wno-implicit-fallthrough -g
YFLAGS= -v -d
LLFLAG= -ll

YACC_SOURCE=grammar.y
LEX_SOURCE=lexer.l
COMPILER_OBJECTS= lex.yy.o y.tab.o linkedlist.o ast.o translate.o

all: lexgrammar yaccgrammar compiler

lexgrammar: $(LEX_SOURCE)
	$(LEX) $(LFLAGS) $^

yaccgrammar: $(YACC_SOURCE)
	$(YACC) $(YFLAGS) $^

compiler: $(COMPILER_OBJECTS)
	$(CC) -o pc $^ $(LYFLAG) -fsanitize=address

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# FIXME
lex.yy.o: lex.yy.c
	$(CC) $(CFLAGS) -c $< -o $@

y.tab.o: y.tab.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf pc *.o lex.yy.c y.tab.c y.output y.tab.h 

.PHONY: all clean
