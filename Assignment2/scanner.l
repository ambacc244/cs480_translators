%{
#include <iostream>
#include <vector>
#include <set>
#include <stack>
#include <cstdlib>

#include "parser.hpp"

std::stack<int> _indent_stack;

YYSTYPE yylval;
YYLTYPE yylloc;
yypstate* pstate = yypstate_new();

#define PUSH_TOKEN(category) do {                      \
    int s = yypush_parse(pstate, category, &yylval, &yylloc);   \
    if (s != YYPUSH_MORE) {                         \
      yypstate_delete(pstate);                      \
      return s;                                     \
    }                                               \
  } while(0)

 #define YY_USER_ACTION \
   yylloc.first_line = yylloc.last_line = yylineno;

%}

%option noyywrap
%option yylineno

%%

%{
  if (_indent_stack.empty()) {
    _indent_stack.push(0);
  }
%}

^[ \t]*\r?\n   { /* Skip blank lines */ }

^[ \t]*#.*\r?\n    { /* Skip whole-line comments. */ }

#.*$          { /* Skip comments on the same line as a statement. */ }

^[ \t]+       {
                /*
                 * Handle indentation as described in Python docs linked above.
                 * Note that this pattern treats leading spaces and leading tabs
                 * equivalently, which could cause some unexpected behavior if
                 * they're combined in a single line.  For the purposes of this
                 * project, that's OK.
                 */
                if (_indent_stack.top() < yyleng) {
                  /*
                   * If the current indentation level is greater than the
                   * previous indentation level (stored at the top of the stack),
                   * then emit an INDENT and push the new indentation level onto
                   * the stack.
                   */
//                  std::cout << "INDENT" << std::endl;
                    yylval.category = INDENT; PUSH_TOKEN(yylval.category);
                  _indent_stack.push(yyleng);
                } else {
                  /*
                   * If the current indentation level is less than or equal to
                   * the previous indentation level, pop indentation levels off
                   * the stack until the top is equal to the current indentation
                   * level.  Emit a DEDENT for each element popped from the stack.
                   */
                  while (!_indent_stack.empty() && _indent_stack.top() != yyleng) {
                    _indent_stack.pop();
//                    std::cout << "DEDENT" << std::endl;
                      yylval.category = DEDENT; PUSH_TOKEN(yylval.category);
                  }

                  /*
                   * If we popped everythin g off the stack, that means the
                   * current indentation level didn't match any on the stack,
                   * which is an indentation error.
                   */
                  if (_indent_stack.empty()) {
                    std::cerr << "Error: Incorrect indentation on line "
                      << yylineno << std::endl;
                    return 1;
                  }
                }
              }

^[^ \t\r\n]+  {
                /*
                 * If we find a line that's not indented, pop all indentation
                 * levels off the stack, and emit a DEDENT for each one.  Then,
                 * call REJECT, so the next rule matching this token is also
                 * applied.
                 */
                while (_indent_stack.top() != 0) {
                  _indent_stack.pop();
//                  std::cout << "DEDENT" << std::endl;
                    yylval.category = DEDENT; PUSH_TOKEN(yylval.category);
                }
                REJECT;
              }

\r?\n         {
                yylval.category = NEWLINE; PUSH_TOKEN(yylval.category);;
              }

<<EOF>>       {
                /*
                 * If we reach the end of the file, pop all indentation levels
                 * off the stack, and emit a DEDENT for each one.
                 */
                while(_indent_stack.top() != 0) {
                  _indent_stack.pop();
//                  std::cout << "DEDENT" << std::endl;
                    yylval.category = DEDENT; PUSH_TOKEN(yylval.category);
                }
                yyterminate();
              }

[ \t]         { /* Ignore spaces that haven't been handled above. */ }

"and"         { yylval.category = AND; PUSH_TOKEN(yylval.category); }
"break"       { yylval.category = BREAK; PUSH_TOKEN(yylval.category); }
"def"         { /* Ignore*/ std::cout << "DEF\t\t" << yytext << std::endl; }
"elif"        { yylval.category = ELIF; PUSH_TOKEN(yylval.category); }
"else"        { yylval.category = ELSE; PUSH_TOKEN(yylval.category); }
"for"         { /* Ignore*/ std::cout << "FOR\t\t" << yytext << std::endl; }
"if"          { yylval.category = IF; PUSH_TOKEN(yylval.category); }
"not"         { yylval.category = NOT; PUSH_TOKEN(yylval.category); }
"or"          { yylval.category = OR; PUSH_TOKEN(yylval.category); }
"return"      { /* Ignore*/ std::cout << "RETURN\t\t" << yytext << std::endl; }
"while"       { yylval.category = WHILE; PUSH_TOKEN(yylval.category); }

"True"        { yylval.category = TRUE; PUSH_TOKEN(yylval.category); }
"False"       { yylval.category = FALSE; PUSH_TOKEN(yylval.category); }

[a-zA-Z_][a-zA-Z0-9_]*  { yylval.str = new std::string(yytext, yyleng); PUSH_TOKEN(IDENTIFIER); }

-?[0-9]*"."[0-9]+       { yylval.str = new std::string(yytext, yyleng); PUSH_TOKEN(FLOAT); }

-?[0-9]+      { yylval.str = new std::string(yytext, yyleng); PUSH_TOKEN(INTEGER); }

"="           { yylval.category = ASSIGN; PUSH_TOKEN(yylval.category); }
"+"           { yylval.category = PLUS; PUSH_TOKEN(yylval.category); }
"-"           { yylval.category = MINUS; PUSH_TOKEN(yylval.category); }
"*"           { yylval.category = TIMES; PUSH_TOKEN(yylval.category); }
"/"           { yylval.category = DIVIDEDBY; PUSH_TOKEN(yylval.category); }

"=="          { yylval.category = EQ; PUSH_TOKEN(yylval.category); }
"!="          { yylval.category = NEQ; PUSH_TOKEN(yylval.category); }
">"           { yylval.category = GT; PUSH_TOKEN(yylval.category); }
">="          { yylval.category = GTE; PUSH_TOKEN(yylval.category); }
"<"           { yylval.category = LT; PUSH_TOKEN(yylval.category); }
"<="          { yylval.category = LTE; PUSH_TOKEN(yylval.category); }

"("           { yylval.category = LPAREN; PUSH_TOKEN(yylval.category); }
")"           { yylval.category = RPAREN; PUSH_TOKEN(yylval.category); }

","           { std::cout << "COMMA\t\t" << yytext << std::endl; }
":"           { yylval.category = COLON; PUSH_TOKEN(yylval.category); }

.             {
                std::cerr << "Unrecognized token on line " << yylineno << ": "
                  << yytext << std::endl;
                return 1;
              }

%%
