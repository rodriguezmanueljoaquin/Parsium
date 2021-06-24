%{
    #include <stdio.h>
    #include <stdbool.h>
    #include "ast.h"
    #include "linkedlist.h"
    #include "translate.h"
    
    extern int yylineno;
    int yylex_destroy();
    int yylex();

    int yywrap() {
	    return 1;
    }

    void yyerror(LinkedList **tree, char const *s) {
        fprintf(stderr, "%s at line %d\n", s, yylineno);
    }
%}


%union  {
    char character; 
    char *id; 
    bool boolean; 
    ValueType valuetype;
    // struct machine *Machine; 
    // struct transition *Transition; 
    // bool (*)(char) Predicate;
    Expression *expression;
    Assignment *assignment;
    Statement *statement;
    LinkedList *linkedlist;
}

%parse-param { LinkedList **tree }

%type <valuetype> type
%type <expression> term expression
%type <statement> definition declaration operation statement assignment
%type <linkedlist> S

%token <id> IDENT/*, parameter, machine_identifier, predicate, state_identifier */
%token DEFINE
%token ASSIGN
/* %token WITH
%token RETURN */
%token <character> CHAR
%token <boolean> BOOL
%token CHAR_DEF BOOL_DEF
%token AND OR EQ NE NOT
%token PRINT

%right ASSIGN
%left  OR
%left  AND
%left  NOT
%nonassoc EQ NE
%nonassoc CHAR_DEF BOOL_DEF

// %type   <Machine> machine
// %type   <Predicate> new_predicate
%start S

%%
S           :   statement               {*tree = newList();}

statement   :   operation ';'           {addToList(*tree, $1);}
            |   statement operation ';' {addToList(*tree, $2);}
            |   ';'                     {;}
            |   statement ';'           {addToList(*tree, $1);}
            |   assignment ';'          {addToList(*tree, $1);}
            ;

operation   :   PRINT CHAR            	{printf("[DEBUG]]: %c", (unsigned char)$2);}
            |   definition              {$$ = $1;}
            |   declaration             {$$ = $1;}
            /* |   return                  {;} */
            ;

declaration :   DEFINE type IDENT   {$$ = newDeclaration($2, $3, NULL);}
            ;

definition  :   DEFINE type IDENT ASSIGN expression     {$$ = newDeclaration($2, $3, $5);}
            // TODO: asignacion a identificadores ya existentes
            ;

assignment  :   IDENT ASSIGN expression          {$$ = newAssignment($1, $3);}
            ;

expression  :   expression OR expression    {$$ = newOperation(OR_OP, $1, $3);}
            |   expression AND expression   {$$ = newOperation(AND_OP, $1, $3);}
            |   expression EQ expression    {$$ = newOperation(EQ_OP, $1, $3);}
            |   expression NE expression    {$$ = newOperation(NE_OP, $1, $3);}
            |   NOT expression              {$$ = newOperation(NOT_OP, $2, NULL);}
            |   '(' expression ')'          {$$ = newOperation(PARENTHESES_OP, $2, NULL);}
            |   term                        {$$ = $1;}
            ;

term        :   BOOL                        {$$ = newBool($1);}
            |   CHAR                        {$$ = newChar($1);}
            |   IDENT                       {$$ = newSymbol($1);}
            ;

type        :   BOOL_DEF                    {$$ = BOOL_TYPE;}
            |   CHAR_DEF                    {$$ = CHAR_TYPE;}
            ;

/* return      :   'return' boolean_exp    {;} */

/* machine     :   '<' machine_param '>'         {$$ = $1;}
            ; */

/* new_predicate   : '(' parameter ')' '{' statement return ';' '}'       {
    bool predicate(char $2) {
        $5 
        $6;
    }

    $$ = predicate;
    }
    ;
    */

/* parse       :   'parse' string 'with' machine_identifier  {;}
            ; */

/* char_array  :   '[' char_array ']'      {;}
            ;

char_array_elem :   character               {;}
            |   character ','                {;}
            |   character ',' char_array_elem    {;}
            ;

transition_array    :   '[' transition_array_elem ']'   {;}
            ;

transition_array_elem   :   transition  {;}
            |   transition ','          {;}
            |   transition_array_elem ',' transition    {;}
            ;

transition  :   state_identifier '->' state_identifier 'when' boolean_exp   {;}
            ; */

/* machine_param     :   'transitions' '=' transition_array ',' 'initialState' '=' state_identifier ',' 'finalStates' '=' state_identifier   {;}
            ; */

/* states_array    :   '[' states_array_elem ']'   {;}
            ;

states_array_elem   :   state_identifier    {;}
            |   state_identifier ','
            |   states_array_elem ',' state_identifier  {;} */
            
%%

int main(int argc, char *argv[]) {
    LinkedList *tree = NULL;
    yyparse(&tree);
    yylex_destroy();
    if(tree != NULL)
        translate(tree);
    return 0;
}
