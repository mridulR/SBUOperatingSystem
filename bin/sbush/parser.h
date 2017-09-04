#ifndef _PARSER_H
#define _PARSER_H

#define MAX_ARGUMENTS 20


typedef struct {
  char * command;
  char * arguments[MAX_ARGUMENTS];
  int argumentCount;
} commandArgument;

commandArgument * parseInput(char* input, char separator);
void freeCommandArgument(commandArgument* cmdArg);

#endif
