
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
[/][*]				            { yy_fatal_error("Unterminated comment"); }
Char                            return CHAR_DEF;
Bool                            return BOOL_DEF;
def                             return DEFINE;
%{
// with                            return WITH;
// return                          return RETURN;
%}
print                           return PRINT;
true                            { yylval.boolean = true; return BOOL;}
false                           { yylval.boolean = false; return BOOL;}

=                               return ASSIGN;
\|\|                            return OR;
&&                              return AND;
==                              return EQ;
!=                              return NE;

!                               return NOT;

[_A-Za-z]([_A-Za-z]|[0-9])*		{ yylval.id = strdup(yytext); return IDENT; }
'.'                             { yylval.character = yytext[1]; return CHAR;}
.                               return yytext[0];

%%
