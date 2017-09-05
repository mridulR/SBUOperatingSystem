#ifndef _PARSER_H
#define _PARSER_H

#include<sys/defs.h>
#define MAX_ARGUMENTS 20

typedef struct {
  char * trimmedInput;
  char * command;
  char * arguments[MAX_ARGUMENTS];
  int isBackground;
  int argumentCount;
} commandArgument;

commandArgument * parseInput(char* input, char separator);
void freeCommandArgument(commandArgument* cmdArg);

#endif
