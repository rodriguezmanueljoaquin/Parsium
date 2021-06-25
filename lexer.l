
%{
    #include <string.h>
    #include <stdbool.h>
    #include <ast.h>
    #include <stdio.h>
    #include "y.tab.h"
%}

%option yylineno

%%

[\n\t ]
\/\/.*\n?
[/][*][^*]*[*]+([^*/][^*]*[*]+)*[/]
[/][*]				            	{ yy_fatal_error("Unterminated comment"); }
Char                            	return CHAR_DEF;
Char\[\]							return CHAR_ARRAY_DEF;
Bool                            	return BOOL_DEF;
def                             	return DEFINE;
Machine							    return MACHINE_DEF;
Predicate                           return PREDICATE_DEF;

transitions                         return TRANSITIONS_DEF;
initialState                        return INITIAL_STATE_DEF;
finalStates                         return FINAL_STATES_DEF;
print                           	return PRINT;
true                            	{ yylval.boolean = true; return BOOL;}
false                           	{ yylval.boolean = false; return BOOL;}
when                                return WHEN;
parse                               return PARSE;
with                                return WITH;
return                              return RETURN;
if                                  return IF;
else                                return ELSE;
for                                 return FOR;
while                               return WHILE;

->                                  return ARROW;

=                               	return ASSIGN;

\|\|                            	return OR;
&&                              	return AND;
==                              	return EQ;
!=                              	return NE;

!                               	return NOT;

\"([^\\\"]|\\.)*\"                  { yylval.string = strndup(&yytext[1], yyleng - 1); yylval.string[yyleng-2] = 0; return STRING; }
[_A-Za-z]([_A-Za-z]|[0-9])*			{ yylval.string = strdup(yytext); return IDENT; }
'.'                             	{ yylval.character = yytext[1]; return CHAR;}
.                               	return yytext[0];

%%
