
%{
    #include <string.h>
    #include <stdbool.h>
    #include <ast.h>
    #include <stdio.h>
    #include "y.tab.h"
	#include <limits.h>
	#include <errno.h>
%}

%option yylineno

%%

[\n\t ]
\/\/.*\n?
[/][*][^*]*[*]+([^*/][^*]*[*]+)*[/]
[/][*]				            	{ yy_fatal_error("Unterminated comment"); }
char                            	return CHAR_DEF;
char\[\]							return CHAR_ARRAY_DEF;
bool                            	return BOOL_DEF;
def                             	return DEFINE;
machine							    return MACHINE_DEF;
predicate                           return PREDICATE_DEF;
int									return INTEGER_DEF;
string								return STRING_DEF;

transitions                         return TRANSITIONS_DEF;
initialState                        return INITIAL_STATE_DEF;
finalStates                         return FINAL_STATES_DEF;
print                        		return PRINT;
read								return READ;

true                            	{ yylval.boolean = true; return BOOL;}
false                           	{ yylval.boolean = false; return BOOL;}
when                                return WHEN;
parse                               return PARSE;
with                                return WITH;
return                              return RETURN;
if                                  return IF;
else                                return ELSE;
while                               return WHILE;

NULL								{yy_fatal_error("NULL is a reserved keyword");}
N									{yy_fatal_error("N is a reserved keyword");}
ANY									return ANY_MACRO;
NO_CHAR								{yy_fatal_error("NO_CHAR is a reserved keyword");}
read_tmp_00							{yy_fatal_error("read_tmp_00 is a reserved keyword");}
read_tmp_01							{yy_fatal_error("read_tmp_01 is a reserved keyword");}
read_tmp_02							{yy_fatal_error("read_tmp_02 is a reserved keyword");};

->                                  return ARROW;

=                               	return ASSIGN;

\|\|                            	return OR;
&&                              	return AND;
==                              	return EQ;
!=                              	return NE;
!                               	return NOT;

>									return GT;
\<									return LT;
>=									return GE;
\<=									return LE;

\+									return PLUS;
-									return MINUS;
\*									return MULT;
\/									return DIV;
%									return MOD;

\<\<								return MACHINE_OPEN;
>>									return MACHINE_CLOSE;

-?[0-9]+								{ 
										char *end = NULL;
										yylval.integer = strtol(yytext, &end, 10);

										if (end == yytext || *end != '\0' || ((yylval.integer == LONG_MAX || yylval.integer == LONG_MIN) && errno == ERANGE)) {
											yy_fatal_error("Integer constant is too large for its type");
										}
										return INTEGER;
									}
\"([^\\\"]|\\.)*\"                  { yylval.string = strndup(&yytext[1], yyleng - 1); yylval.string[yyleng-2] = 0; return STRING; }
[_A-Za-z]([_A-Za-z]|[0-9])*			{ yylval.string = strdup(yytext); return IDENT; }
'\\n'                             	{ yylval.character = '\n'; return CHAR;}
'\\r'                             	{ yylval.character = '\r'; return CHAR;}
'\\t'                             	{ yylval.character = '\t'; return CHAR;}
'.'                             	{ yylval.character = yytext[1]; return CHAR;}
.                               	return yytext[0];

%%
