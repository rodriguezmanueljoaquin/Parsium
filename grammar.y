%{
    #include <stdio.h>
    #include <stdbool.h>
    #include "grammar.h"
%}

%union  {struct Value Value; char Char; char *id; char *String; bool Bool; struct machine *Machine; struct transition *Transition; bool (*)(char) Predicate;}
%token  <id> identifier/*, parameter, machine_identifier, predicate, state_identifier */
%token  <Char> character_value
// %token  <String> string
%token  <Bool> boolean_value
// %type   <Machine> machine
// %type   <Predicate> new_predicate
%type   <Value> term, character, boolean
%start  statement

%%
statement   :   operation ';'           {;}
            |   statement operation ';'   {;}
            |   ';'                     {;}
            ;

operation   :   assignment              {;}
            |   'print' term            {printf("%s", $1);}
            |   definition              {;}
            |   declaration             {;}
            |   return                  {;}
            ;

declaration :   'def' type identifier   {createEmptySymbol($2, $3);}
            ;

definition  :   'def' type identifier '=' term     {createSymbol($1, $2, $4);}
            // TODO: asignacion a identificadores ya existentes
            ;

assignment  :   identifier '=' term     {updateSymbolVal($1, $3);}
            ;

return      :   'return' boolean_exp    {;}
            ;

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
term        :   character               {$$ = $1;}
            // |   string                  {$$ = $1;}
            |   boolean_exp                 {$$ = $1;}
            // |   machine                 {$$ = $1;}
            // |   transition              {$$ = $1;}
            // |   predicate               {$$ = $1;}
            ; 

character   :   character_value         {struct charValue = {&($1), CHAR_TYPE}; // ojo, no se pierde la informacion de $1 ?
                                        $$ = charValue;}
            |   identifier              {$$ = symbolVal($1);}
            ;

boolean_exp :   boolean_exp '||' bool1  {;}
            |   bool1                   {;}
            /* |   parse                   {;} */
            ;

bool1       :   bool1 '&&' bool2        {;}
            |   bool2                   {;}
            ;

bool2       :   '(' boolean_exp ')'     {;}
            |   '!' boolean_exp         {;}
            |   boolean_term            {;}
            ;

boolean_term    :   boolean             {;}
            /* |   predicate '(' character ')' {;} */
            ;

boolean     :   boolean_value         {struct boolValue = {&($1), BOOL_TYPE}; // ojo, no se pierde la informacion de $1 ?
                                        $$ = boolValue;}
            |   identifier              {$$ = symbolVal($1);}
            ;

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
            ;

type        :   'Machine'               {;}
            |   'Machine[]'             {;}
            |   'Predicate'             {;}
            |   'Predicate[]'           {;}
            |   'Bool'                  {;}
            |   'Bool[]'                {;}
            |   'Char'                  {;}
            |   'Char[]'                {;}
            |   'Transition'            {;}
            |   'Transition[]'          {;}
            |   'String'                {;}
            |   'String[]'              {;}
            |   'State'                 {;}
            |   'State[]'               {;}
            ;
            
%%

int main(int argc, char *argv[]) {
    yyparse();
}

void createEmptySymbol(char *typeStr, char *identifier) {
    
}

void createSymbol(char *typeStr, char *identifier, Value val) {
    
}

void symbolVal(char *identifier) {

}