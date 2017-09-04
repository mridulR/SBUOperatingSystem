#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

#define DEBUG 1
#define ARGUMENT_LENGTH 20
#define COMMAND_LENGTH 20
#define INPUT_LENGTH 500

void freeCommandArgument(commandArgument* cmdArg)
{
  if((*cmdArg).command)
  {
    free((*cmdArg).command);
  }
  for(int i = 0; i < (*cmdArg).argumentCount; ++i)
  {
    if((*cmdArg).arguments[i])
    {
      free((*cmdArg).arguments[i]);
    }
  }
}

char * trim(char * input) {
  char * trimmedInput = malloc(sizeof(char) * INPUT_LENGTH);
  int count = 0;
  int left_index = 0;
  int right_index = strlen(input) - 1;
  
  while (input[left_index] == ' ' && (left_index <= right_index)) {
   left_index++;
  }

  while ((input[right_index] == ' ' || input[right_index] == '\n')  && (left_index <= right_index)) {
   right_index--;
  }


  for (int index = left_index; index <= right_index; index++) {
   char ch = input[index];
     trimmedInput[count] = ch;
     count++;
  }
  return trimmedInput;
}

commandArgument * parseInput(char * inputLine,  char separator) {
  char * input = trim(inputLine);
  commandArgument * c_arg = malloc(sizeof(commandArgument));
  (*c_arg).command = malloc(sizeof(char) * COMMAND_LENGTH);
  char * command = (*c_arg).command;
  char * argument = NULL;
  char ch;
  int count = 0;
  int command_flag = 1;
  while(*input) {
    ch = *input;
    
    /*


    */
    if (command_flag == 1 && ch != separator) {
      *command = ch;
      command++;
      *command = '\0';
    }

    if (command_flag == 1 && ch == separator) {
      command_flag = 0;
    }


    if (ch == separator && command_flag == 0) {
      if (count > MAX_ARGUMENTS) {
        #ifdef DEBUG
         fputs("Too many arguments passed" ,stdout);
        #endif
        return NULL;
      }
      
      if (argument == NULL || strlen((*c_arg).arguments[count - 1]) != 0) {
       *((*c_arg).arguments + count) = malloc(sizeof(char) * ARGUMENT_LENGTH);
       argument = *((*c_arg).arguments + count);
       count++;       
      }
    }

    if (ch != separator && command_flag == 0) {
     *argument = ch;
     *(argument + 1) = '\0';
     argument++; 
    }

    input++;
  }

  (*c_arg).argumentCount = count;
  return c_arg;
}
