#include <iostream>
#include <set>
#include "parser.hpp"

extern int yylex();

extern std::string* target_program;
extern std::set<std::string> symbols;

int main() {
  if (!yylex()) {
    // Write initial C++ stuff.
    std::cout << "digraph G {" << std::endl;
    std::cout << " n0 [label="Block"];" << std::endl;

    // Write declaractions for all variables.
    std::set<std::string>::iterator it;
    for (it = symbols.begin(); it != symbols.end(); it++) {
      std::cout << "double " << *it << ";" << std::endl;
    }

    // Write print statements for all symbols.
    for (it = symbols.begin(); it != symbols.end(); it++) {
      std::cout << "std::cout << \"" << *it << ": \" << " << *it << " << std::endl;" << std::endl;
    }

    // Write terminating brace.
    std::cout << "}" << std::endl;

    delete target_program;
    target_program = NULL;

  }
}
