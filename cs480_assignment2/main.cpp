#include <iostream>
#include <map>
#include <string>
#include <list>

extern int yylex();
extern bool _error;
extern std::list<std::string> codes;
extern std::map<std::string, float> symbols;

int main() {

  if (!yylex()) {
    std::cout << "#include <iostream>" << std::endl;
    std::cout << "int main(){\n" << std::endl;

    std::map<std::string, float>::iterator it;
    for (it = symbols.begin(); it != symbols.end(); it++)
      std::cout << "double "<< it->first << ";" << std::endl;


    std::cout << "\n/* Begin program */\n" << std::endl;

    std::list<std::string>::iterator it2;

    for (it2 = codes.begin(); it2 != codes.end(); ++it2)
      std::cout << *it2 << std::endl;

    std::cout << "\n/* End program */\n" << std::endl;

    for (it = symbols.begin(); it != symbols.end(); it++)
      std::cout << "std::cout << \"" << it->first << ": \" << " << it->first << " << std::endl;" << std::endl;


    std::cout << "}" << std::endl;
    return 0;
  } else {
    return 1;
  }
}
