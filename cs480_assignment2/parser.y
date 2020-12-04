%{
#include <iostream>
#include <map>
#include <string>
#include <list>
#include "parser.hpp"

bool _error = false;
std::string start;
std::list<std::string> codes;
std::map<std::string, float> symbols;
int i = 0;
extern int yylex();
void yyerror(YYLTYPE* loc, const char* err) {
  std::cerr << "Error (line " << loc->first_line << "): " << err << std::endl;
}
%}


%locations
%define parse.error verbose

%define api.pure full
%define api.push-pull push

/* %define api.value.type { std::string* } */
%union {
  std::string* str;
  float val;
  int category;
}

%token <str> IDENTIFIER
%token <str> INTEGER
%token <str> FLOAT
%token <category> ASSIGN PLUS MINUS TIMES DIVIDEDBY
%token <category> EQ NEQ GT GTE LT LTE
%token <category> LPAREN RPAREN COMMA COLON
%token <category> TRUE FALSE
%token <category> IF ELIF ELSE WHILE
%token <category> DEDENT INDENT
%token <category> BREAK
%token <category> NEWLINE
%token <category> AND OR
%token <category> NOT


%type <str> expression
%type <str> comparison
%type <str> state
%type <str> term


%left PLUS MINUS
%left TIMES DIVIDEDBY
%left EQ NEQ GT GTE LT LTE
%left AND OR

%start program

%%

program
  : program statement
  | statement
  ;

statement
  : IDENTIFIER ASSIGN expression NEWLINE {
      std::string* assign = new std::string(*$1 + " = " + *$3 + ";");
      start = *assign;
  //    std::cout << start << std::endl;
      codes.push_back(start);
      symbols[*$1] = 0; }
  | IF state COLON NEWLINE INDENT {
      std::string* If = new std::string("if ( " + *$2 + " ) {");
      start = *If;
  //    std::cout << start << std::endl;
      codes.push_back(start); }
 | ELIF state COLON NEWLINE INDENT {
      std::string* Elif = new std::string("else if ( " + *$2 + " ){");
      start = *Elif;
  //    std::cout << start << std::endl;
      codes.push_back(start); }
  | ELSE COLON NEWLINE INDENT {
      std::string* Else = new std::string("else {");
      start = *Else;
  //    std::cout << start << std::endl;
      codes.push_back(start); }
  | WHILE state COLON NEWLINE INDENT {
      std::string* While = new std::string("while ( " + *$2 + " ) {");
      start = *While;
  //    std::cout << start << std::endl;
      codes.push_back(start); }
  | BREAK NEWLINE {
      std::string* Break = new std::string("break;");
      start = *Break;
  //    std::cout << start << std::endl;
      codes.push_back(start); }
  | DEDENT {
      std::string* dedent = new std::string("}");
      start = *dedent;
  //    std::cout << start << std::endl;
      codes.push_back(start); }
  ;

expression
  : expression PLUS term {
      std::string* plus =  new std::string(*$1 + " + " + *$3);
      $$ = plus; }
  | expression MINUS term {
      std::string* minus = new std::string(*$1 + " - " + *$3);
      $$ = minus; }
  | expression TIMES term {
      std::string* times = new std::string(*$1 + " * " + *$3);
  //    std::cout << times << std::endl;
      $$ = times; }
  | expression DIVIDEDBY term {
      std::string* dividedby = new std::string(*$1 + " / " + *$3);
      $$ = dividedby; }
  | term { $$ = $1; }
  ;

term
  : LPAREN expression RPAREN {
      std::string *paren =  new std::string("( " + *$2 + " )");
  //    std::cout << paren << std::endl;
      $$ = paren; }
  | IDENTIFIER { $$ = $1; }
  | INTEGER { $$ = $1; }
  | FLOAT { $$ = $1; }
  | TRUE {
      std::string* True = new std::string("true");
      $$ = True; }
  | FALSE {
      std::string* False = new std::string("false");
      $$ = False; }
  ;

state
  : state AND state {
      std::string* And = new std::string(*$1 + " && " + *$3);
      $$ = And; }
  | state OR state {
      std::string* Or = new std::string(*$1 + " || " + *$3);
      $$ = Or; }
  | comparison EQ comparison {
      std::string* eq = new std::string(*$1 + " == " + *$3);
      $$ = eq; }
  | comparison NEQ comparison {
      std::string* neq = new std::string( *$1 + " != " + *$3);
      $$ = neq; }
  | comparison GT comparison {
      std::string* gt = new std::string( *$1 + " > " + *$3);
      $$ = gt; }
  | comparison GTE comparison {
      std::string* gte = new std::string( *$1 + " >= " + *$3);
      $$ = gte; }
  | comparison LT comparison {
      std::string* lt = new std::string( *$1 + " < " + *$3);
      $$ = lt; }
  | comparison LTE comparison {
      std::string* lte = new std::string( *$1 + " <= " + *$3);
      $$ = lte; }
  | comparison { $$ = $1; }
  ;

comparison
  : IDENTIFIER { $$ = $1; }
  | INTEGER { $$ = $1; }
  | FLOAT { $$ = $1; }
  | NOT comparison {
     std::string* Not =  new std::string("!" + *$2);
     $$ = Not; }
  | TRUE {
     std::string* True = new std::string("true");
     $$ = True; }
  | FALSE {
      std::string* False = new std::string("false");
     $$ = False; }
  ;

%%
