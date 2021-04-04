#include "ArgParser.hpp"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
ArgParser::ArgParser() {
    SetDefaultValues();
}
// similar to assignment 4
ArgParser::ArgParser(int argc, const char* argv[]) {
  SetDefaultValues();

  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-pcf")) {
        i++;
        draw_pcf = true;
    }
    else {
        printf("Unknown command line argument %d: '%s'\n", i, argv[i]);
        exit(1);
    }
  }

std::cout << "Args:\n";

  std::cout << "- pcf : " << draw_pcf  << std::endl;
}

void ArgParser::SetDefaultValues() {
  draw_pcf = false;
}
