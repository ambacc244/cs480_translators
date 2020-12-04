%{
#include <iostream>
#include <vector>
#include <set>

struct _word {
  std::string lexeme;
  std::string category;
};

std::vector<struct _word> _words;
std::set<std::string> _ids;
bool _error = false;
int current_line_indent = 0;
int indent_level = 0;

void _save_word(std::string lexeme, std::string category) {
  struct _word word = { .lexeme = lexeme, .category = category};
  _words.push_back(word);
}

%}

%option noyywrap
%option yylineno

%x indent

DIGIT             [0-9]
INTEGER           {DIGIT}+
FLOAT             {INTEGER}"."{DIGIT}*
IDENTIFIER        [a-zA-Z_][a-zA-Z_0-9]*

%%

"#".*\n             {}

<indent>\t        { current_line_indent++; }
<indent>" "       { current_line_indent++; }
<indent>.         {
                    unput(*yytext);
                    if(current_line_indent/4 > indent_level) {
                        for(int i=0; i<(current_line_indent/4 - indent_level);i++)
                          _save_word("", "INDENT");
                        indent_level = current_line_indent/4;
                    }
                    else if (current_line_indent/4 < indent_level) {
                        for(int i=0; i<(indent_level - current_line_indent/4);i++)
                          _save_word("", "DEDENT");
                        indent_level = current_line_indent/4;
                    }
                    else {
                       BEGIN(INITIAL);
                    }
                  }

<*>\n+             {
                    _save_word("", "NEWLINE");
                    current_line_indent = 0;
                    BEGIN(indent);
                  }



"True"            { _save_word("1", "BOOLEAN"); }
"False"           { _save_word("0", "BOOLEAN"); }

"and"             { _save_word(yytext, "AND"); }
"break"           { _save_word(yytext, "BREAK"); }
"def"             { _save_word(yytext, "DEF"); }
"elif"            { _save_word(yytext, "ELIF"); }
"else"            { _save_word(yytext, "ELSE"); }
"for"             { _save_word(yytext, "FOR"); }
"if"              { _save_word(yytext, "IF"); }
"not"             { _save_word(yytext, "NOT"); }
"or"              { _save_word(yytext, "OR"); }
"return"          { _save_word(yytext, "RETURN"); }
"while"           { _save_word(yytext, "WHILE"); }

"="               { _save_word(yytext, "ASSIGN"); }
"+"               { _save_word(yytext, "PLUS"); }
"-"               { _save_word(yytext, "MINUS"); }
"*"               { _save_word(yytext, "TIMES"); }
"/"               { _save_word(yytext, "DIVIDEDBY"); }
"=="              { _save_word(yytext, "EQ"); }
"!="              { _save_word(yytext, "NEQ"); }
">"               { _save_word(yytext, "GT"); }
">="              { _save_word(yytext, "GTE"); }
"<"               { _save_word(yytext, "LT"); }
"<="              { _save_word(yytext, "LTE"); }

"("               { _save_word(yytext, "LPAREN"); }
")"               { _save_word(yytext, "RPAREN"); }
","               { _save_word(yytext, "COMMA"); }
":"               { _save_word(yytext, "COLON"); }

{IDENTIFIER}      {
                    _save_word(yytext, "IDENTIFIER");
                    _ids.insert(yytext);
                  }

{INTEGER}         { _save_word(yytext, "INTEGER"); }

{FLOAT}           { _save_word(yytext, "FLOAT"); }

[ ]*              {}

.                 {
                    std::cerr << "Invalid symbol on line " << yylineno << " : "<< yytext << std::endl;
                    _error = true;
                  }

<<EOF>>           {
                    if (_error)
                      return 1;
                    else
                      return 0;
                  }
%%

int main(){

  if(!yylex()){
    std::cout << "Saw these tokens:" << std::endl;
    for(int i = 0; i < _words.size(); i++){
      std::cout << _words[i].category << " " << _words[i].lexeme << std::endl;
    }
  }
}