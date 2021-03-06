%{
    #include <stdio.h>
    #include <stdbool.h>
    #include <ast.h>
    #include <linkedlist.h>
    #include <translate.h>
    
    enum {TREE_LIST, MACHINE_LIST, LIST_COUNT};
    
    extern int yylineno;

	extern LinkedList *predicates;
	extern LinkedList *variableScopes;
    extern LinkedList *globalVaribles;

    int yylex_destroy();
    int yylex();

    int yywrap() {
	    return 1;
    }

    void yyerror(LinkedList **args, char const *s) {
        fprintf(stderr, "%s at line %d\n", s, yylineno);
    }
%}

%union  {
    char character; 
    char *string; 
    bool boolean;
	long integer;
    ValueType valuetype;
    Expression *expression;
    Assignment *assignment;
    Statement *statement;
    LinkedList *linkedlist;
    Transition *transition;
    TransitionCondition *transitionCondition;
}

//  FIXME: cambiar esto por variables globales?
%parse-param { LinkedList **args }

%type <valuetype> type
%type <expression> term expression binary_operation unary_operation machine predicate
%type <statement> definition declaration basic_statement statement assignment block
%type <linkedlist> S statement_list transition_array_elem transition_array final_state_array_elem final_state_array
%type <transition> transition
%type <transitionCondition> transition_when

%token <string> IDENT STRING
%token <integer> INTEGER
%token DEFINE
%token ASSIGN
%token <character> CHAR
%token <boolean> BOOL
%token CHAR_DEF BOOL_DEF CHAR_ARRAY_DEF MACHINE_DEF TRANSITIONS_DEF INITIAL_STATE_DEF FINAL_STATES_DEF PREDICATE_DEF INTEGER_DEF STRING_DEF
%token AND OR EQ NE NOT
%token PRINT READ
%token RETURN
%token MACHINE_OPEN MACHINE_CLOSE
%token ARROW
%token WHEN PARSE WITH
%token IF ELSE WHILE
%token ANY_MACRO

%right ASSIGN
%left  OR
%left  AND
%left  NOT
%nonassoc LT GT LE GE EQ NE
%left PLUS MINUS
%left MULT DIV MOD
%nonassoc CHAR_DEF BOOL_DEF

%start S

%%

S 			:	S statement								{if($2 != NULL) addToList($1, $2);}
			|	/* empty */								{   args[TREE_LIST] = newList(); 
                                                            args[MACHINE_LIST] = newList(); 
                                                            $$ = args[TREE_LIST];
                                                        }
			;

statement_list	: statement_list statement 				{if($2 != NULL) addToList($1, $2);}
			|	/*empty*/								{$$ = newList();}
			;

statement   :   basic_statement ';'                     {$$ = $1;}
            |   ';'                                     {;}
            |   assignment ';'                          {$$ = $1;}
            |   expression ';'                          {$$ = newStatement(EXPRESSION_STMT, $1);}
			|	if_declaration '(' expression ')' block ELSE {pushScope();} block        {$$ = newConditional($3, $5, $8);}
			|	if_declaration '(' expression ')' block                                  {$$ = newConditional($3, $5, NULL);}
            |   WHILE {pushScope();} '(' expression ')' block                               {$$ = newLoop($4, $6, "\0", "\0");}
            ;

if_declaration  : IF                                    {pushScope();} 
                ;

block       :   '{' statement_list '}'	                {$$ = newBlock($2); popScope();}
            ;

basic_statement     :   declaration                             {$$ = $1;}
                    |   definition                              {$$ = $1;}
                    //TODO: Validar que retorne algo booleano
                    |   RETURN expression                       {$$ = newStatement(RETURN_STMT, $2);}
                    ;

declaration :   DEFINE type IDENT                       {$$ = newDeclaration($2, $3, NULL);}
            ;

definition  :   DEFINE type IDENT ASSIGN expression     {
                                                            $$ = newDeclaration($2, $3, $5); 
                                                            if($2 == MACHINE_TYPE) {
                                                                addToList(args[MACHINE_LIST], $$); 
                                                                $$ = NULL; // para que no se guarde en TREE_LIST
                                                            }
                                                        }
            |   DEFINE PREDICATE_DEF {pushScope();} IDENT ASSIGN '(' IDENT {newDeclaration(CHAR_TYPE, $7, NULL);} ')' block  {
                                                                                                                                    $$ = NULL;
                                                                                                                                    newPredicate($4, $7, $10);
                                                                                                                            }
            ;

assignment  :   IDENT ASSIGN expression                 {$$ = newAssignment($1, $3);}
            ;

expression  :   binary_operation						{$$ = $1;}
            |   unary_operation                         {$$ = $1;}
            |   predicate                               {$$ = $1;}
            |   machine                                 {$$ = $1;}
            |   PARSE term WITH IDENT                   {$$ = newParseExpression($4, $2);}
            ;

predicate   :   IDENT '(' IDENT ')'                     {$$ = newPredicateCall($1, $3, 0);}
            |   IDENT '(' CHAR ')'                      {$$ = newPredicateCall($1, NULL, $3);}
            ;

binary_operation    :   expression OR expression        {$$ = newExpression(OR_OP, $1, $3);}
                    |   expression AND expression       {$$ = newExpression(AND_OP, $1, $3);}
                    |   expression EQ expression        {$$ = newExpression(EQ_OP, $1, $3);}
                    |   expression NE expression        {$$ = newExpression(NE_OP, $1, $3);}
                    |   expression PLUS expression      {$$ = newExpression(PLUS_OP, $1, $3);}
                    |   expression MINUS expression     {$$ = newExpression(MINUS_OP, $1, $3);}
                    |   expression MULT expression      {$$ = newExpression(MULT_OP, $1, $3);}
                    |   expression DIV expression       {$$ = newExpression(DIV_OP, $1, $3);}
                    |   expression MOD expression       {$$ = newExpression(MOD_OP, $1, $3);}
                    |   expression GT expression        {$$ = newExpression(GT_OP, $1, $3);}
                    |   expression LT expression        {$$ = newExpression(LT_OP, $1, $3);}
                    |   expression GE expression        {$$ = newExpression(GE_OP, $1, $3);}
                    |   expression LE expression        {$$ = newExpression(LE_OP, $1, $3);}
                    ;

unary_operation     :   '(' expression ')'                  {$$ = newExpression(PARENTHESES_OP, $2, NULL);}
                    |   NOT expression                      {$$ = newExpression(NOT_OP, $2, NULL);}
                    |   MINUS expression                    {$$ = newExpression(MINUS_OP, NULL, $2);}
                    |   PLUS expression                     {$$ = newExpression(PLUS_OP, NULL, $2);}
                    |   PRINT '(' expression ')'            {$$ = newExpression(PRINT_OP, $3, NULL);}
                    |   READ '(' ')'                        {$$ = newExpression(READ_OP, NULL, NULL);}
                    |   term                                {$$ = $1;}
                    ;

term        :   BOOL                                    {$$ = newBool($1);}
            |   IDENT                                   {$$ = newSymbol($1);}
            |   CHAR                                    {$$ = newChar($1);}
            |   STRING                                  {$$ = newString($1);}
			|	INTEGER									{$$ = newInteger($1);}
            ;

type        :   BOOL_DEF                                {$$ = BOOL_TYPE;}
            |   CHAR_DEF                                {$$ = CHAR_TYPE;}
            |   CHAR_ARRAY_DEF                          {$$ = CHAR_ARRAY_TYPE;}
            |   MACHINE_DEF                             {$$ = MACHINE_TYPE;}
			|	INTEGER_DEF								{$$ = INTEGER_TYPE;}
            |   STRING_DEF                              {$$ = STRING_TYPE;}
			;

machine     :   MACHINE_OPEN TRANSITIONS_DEF ASSIGN transition_array ',' INITIAL_STATE_DEF ASSIGN IDENT ',' FINAL_STATES_DEF ASSIGN final_state_array MACHINE_CLOSE {$$ = newMachine($4, $8, $12);}
            ;

transition_array    :   '[' transition_array_elem ']'   {$$ = $2;}
            ;

transition_array_elem   :   transition                  {$$ = newList(); addToList($$, $1);}
            |   transition_array_elem ',' transition    {addToList($$,$3);}
            ;

transition  :   IDENT ARROW IDENT WHEN transition_when      {$$ = newTransition($1, $3, $5);}
            ;

transition_when :   IDENT                                   {$$ = newTransitionCondition($1, 0);}
                |   CHAR                                    {$$ = newTransitionCondition(NULL, $1);}
                |   ANY_MACRO                               {$$ = newTransitionCondition(NULL, 0);}
                ;

final_state_array :   '[' final_state_array_elem ']'                {$$ = $2;}
            ;

final_state_array_elem   :   IDENT                            {$$ = newList(); addToList($$, $1);}
            |   final_state_array_elem ',' IDENT              {addToList($$,$3);}
            ;

%%
int main(int argc, char *argv[]) {
	predicates = newList();
    variableScopes = newList();
    globalVaribles = newList();
    pushScope();
    LinkedList *args[LIST_COUNT] = {0};
    yyparse(args);
    yylex_destroy();
    translate(args[TREE_LIST], args[MACHINE_LIST], predicates, globalVaribles);
    return 0;
}
