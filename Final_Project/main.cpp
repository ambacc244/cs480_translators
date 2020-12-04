#include <iostream>
#include <set>
#include <fstream>

#include "ast.hpp"
#include "parser.hpp"

extern int yylex();

extern ASTBlock* programBlock;
extern std::set<std::string> symbols;

int main() {
  if (!yylex()) {
    if (programBlock) {
      std::cout << "; ModuleID = 'Python compiler'\n";
      std::cout << "source_filename = \"Python compiler\"";
      std::cout << "\n\ndefine internal void @main() {\n";
      std::cout << "entry:\n";
      std::string result = generateGVSpec(programBlock);

      for( std::set<std::string>::reverse_iterator iter = symbols.rbegin() ; iter != symbols.rend() ; ++iter)
        std::cout << "  %" << *iter << " = alloca float\n";

      std::cout << result;
    }
  }
}
