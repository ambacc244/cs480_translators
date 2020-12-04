#include <iostream>

extern "C" {
 float main();
}

int main(int argc, char const *argv[]) {
  float c = main();
  return 0;
}
