
%{
    #include <string.h>
    #include <stdbool.h>
    #include <ast.h>
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
%{
// with                            return WITH;
// return                          return RETURN;
%}
transitions                         return TRANSITIONS_DEF;
initialState                        return INITIAL_STATE_DEF;
finalStates                         return FINAL_STATES_DEF;
print                           	return PRINT;
true                            	{ yylval.boolean = true; return BOOL;}
false                           	{ yylval.boolean = false; return BOOL;}
when                                return WHEN;

->                                  return ARROW;

=                               	return ASSIGN;

\|\|                            	return OR;
&&                              	return AND;
==                              	return EQ;
!=                              	return NE;

!                               	return NOT;

[_A-Za-z]([_A-Za-z]|[0-9])*			{ yylval.id = strdup(yytext); return IDENT; }
'.'                             	{ yylval.character = yytext[1]; return CHAR;}
.                               	return yytext[0];

%%
