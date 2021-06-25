%{
    #include <stdio.h>
    #include <stdbool.h>
    #include <ast.h>
    #include <linkedlist.h>
    #include <translate.h>
    
    enum {TREE_LIST, MACHINE_LIST, LIST_COUNT};
    
    extern int yylineno;
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
    ValueType valuetype;
    // struct machine *Machine; 
    // struct transition *Transition; 
    // bool (*)(char) Predicate;
    Expression *expression;
    Assignment *assignment;
    Statement *statement;
    LinkedList *linkedlist;
    TransitionType *transitiontype;
}

%parse-param { LinkedList **args }

%type <valuetype> type
%type <expression> term expression array machine
%type <statement> definition declaration operation statement assignment block
%type <linkedlist> S statement_list char_array_elem char_array transition_array_elem transition_array ident_array_elem ident_array
%type <transitiontype> transition

%token <string> IDENT STRING
%token DEFINE
%token ASSIGN
/* %token WITH
%token RETURN */
%token <character> CHAR
%token <boolean> BOOL
%token CHAR_DEF BOOL_DEF CHAR_ARRAY_DEF MACHINE_DEF TRANSITIONS_DEF INITIAL_STATE_DEF FINAL_STATES_DEF PREDICATE_DEF
%token AND OR EQ NE NOT
%token PRINT
%token RETURN
%token ARROW
%token WHEN PARSE WITH
%token IF ELSE FOR WHILE

%right ASSIGN
%left  OR
%left  AND
%left  NOT
%nonassoc EQ NE
%nonassoc CHAR_DEF BOOL_DEF

%start S

%%

S 			:	S statement								{if($2 != NULL) addToList($1, $2);}
			|	/* empty */								{args[TREE_LIST] = newList(); args[MACHINE_LIST] = newList(); $$ = args[TREE_LIST];}
			;

statement_list	: statement_list statement 				{if($2 != NULL) addToList($1, $2);}
			|	/*empty*/								{$$ = newList();}
			;

statement   :   operation ';'                           {$$ = $1;}
            |   ';'                                     {;}
            |   assignment ';'                          {$$ = $1;}
            |   expression ';'                          {$$ = newStatement(EXPRESSION_STMT, $1);}
			|	IF '(' expression ')' block ELSE block  {$$ = newConditional($3, $5, $7);}
			|	IF '(' expression ')' block             {$$ = newConditional($3, $5, NULL);}
            |   WHILE '(' expression ')' block          {$$ = newLoop($3, $5, "\0", "\0");}
            ;

block       :   '{' statement_list '}'					{$$ = newBlock($2);}
            ;

operation   :   PRINT CHAR            	                {printf("// [DEBUG]: %c\n", (unsigned char)$2);}
            |   declaration                             {$$ = $1;}
            |   definition                              {$$ = $1;}
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
            ;

assignment  :   IDENT ASSIGN expression                 {$$ = newAssignment($1, $3);}
            ;

expression  :   expression OR expression                {$$ = newExpression(OR_OP, $1, $3);}
            |   expression AND expression               {$$ = newExpression(AND_OP, $1, $3);}
            |   expression EQ expression                {$$ = newExpression(EQ_OP, $1, $3);}
            |   expression NE expression                {$$ = newExpression(NE_OP, $1, $3);}
            |   NOT expression                          {$$ = newExpression(NOT_OP, $2, NULL);}
            |   '(' expression ')'                      {$$ = newExpression(PARENTHESES_OP, $2, NULL);}
			|	array						            {$$ = $1;}
            |   machine                                 {$$ = $1;}
            |   term                                    {$$ = $1;}
            |   PARSE STRING WITH IDENT                 {$$ = newExpression(PARSE_OP, newSymbol($4), newSymbol($2));}
            ;

term        :   BOOL                                    {$$ = newBool($1);}
            |   CHAR                                    {$$ = newChar($1);}
            |   IDENT                                   {$$ = newSymbol($1);}
            |   STRING                                  {$$ = newSymbol($1);}
            ;

type        :   BOOL_DEF                                {$$ = BOOL_TYPE;}
            |   CHAR_DEF                                {$$ = CHAR_TYPE;}
            |   CHAR_ARRAY_DEF                          {$$ = CHAR_ARRAY_TYPE;}
            |   MACHINE_DEF                             {$$ = MACHINE_TYPE;}
			;

array		:	char_array					            {$$ = newArray($1, CHAR_ARRAY_TYPE);}

char_array  :  '[' char_array_elem ']'                  {$$ = $2;}
            ;

char_array_elem :   term               				    {$$ = newList(); addToList($$, $1);}
            |   char_array_elem ',' term    		    {addToList($$,$3);}
            ;

machine     :   '<' TRANSITIONS_DEF ASSIGN transition_array ',' INITIAL_STATE_DEF ASSIGN IDENT ',' FINAL_STATES_DEF ASSIGN ident_array '>' {$$ = newMachine($4, $8, $12);}
            ;

transition_array    :   '[' transition_array_elem ']'   {$$ = $2;}
            ;

transition_array_elem   :   transition                  {$$ = newList(); addToList($$, $1);}
            |   transition_array_elem ',' transition    {addToList($$,$3);}
            ;

transition  :   IDENT ARROW IDENT WHEN expression      {$$ = newTransition($1, $3, $5);}
            ;

ident_array :   '[' ident_array_elem ']'                {$$ = $2;}
            ;

ident_array_elem   :   IDENT                            {$$ = newList(); addToList($$, $1);}
            |   ident_array_elem ',' IDENT              {addToList($$,$3);}
            ;
/* return      :   'return' boolean_exp    {;} */

/* new_predicate   : '(' parameter ')' '{' statement return ';' '}'       {
    bool predicate(char $2) {
        $5 
        $6;
    }

    $$ = predicate;
    }
    ;
    */

%%

int main(int argc, char *argv[]) {
    LinkedList *args[LIST_COUNT] = {0};
    yyparse(args);
    yylex_destroy();
    translate(args[TREE_LIST], args[MACHINE_LIST]);
    return 0;
}
