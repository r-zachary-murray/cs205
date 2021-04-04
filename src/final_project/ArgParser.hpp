#ifndef ARG_PARSER_H_
#define ARG_PARSER_H_

#include <string>

class ArgParser {
 public:
  ArgParser();
  ArgParser(int argc, const char* argv[]);
  bool draw_pcf;


 private:
  void SetDefaultValues();

};

#endif  // ARG_PARSER_H
