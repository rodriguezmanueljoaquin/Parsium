
%{
    #include <string.h>
%}

%option yylineno

%%

[\n\t ]
\/\/.*\n?
[/][*][^*]*[*]+([^*/][^*]*[*]+)*[/]
[/][*]				            { yy_fatal_error("Unterminated comment"); }

Char                            return CHAR;
Bool                            return BOOL;
def                             return DEFINE;
with                            return WITH;
return                          return RETURN;

=                               return ASSIGN;
\|\|                            return OR;
&&                              return AND;
==                              return EQ;
!=                              return NE;

true                            return TRUE;
false                           return FALSE;
!                               return NOT;

[_A-Za-z]([_A-Za-z]|[0-9])*		{ yylval.id = strdup(yytext); return IDENT; }
'.'                             { yylval.Value.valType = CHAR_TYPE; yylval.Value.valPtr = (void *)strndup(&yytext[1], 1);}
true                            { yylval.Value.valType = BOOL_TYPE; yylval.Value.valPtr = }
false                           
.                               return yytext[0];

%%
