#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "parser.h"

#define BUILT_IN_CD 1
#define BUILT_IN_PWD 2
#define TRUE 1
#define FALSE 0
//#define DEBUG TRUE
#define BUFFER_SIZE 1000

int execvpe(const char *file, char *const argv[], char *const envp[]);
pid_t waitpid(pid_t pid, int *stat_loc, int options);

void executeBinaryInteractively(commandArgument *c_arg) {
  int pid = fork();
  int waitStatus =0;
  char *path;
  path  = getenv("PATH");
  char *envp[] = {path, NULL};
  char *argv[(*c_arg).argumentCount + 2];
  argv[0] = (*c_arg).command;
  int index = 0;
  for (; index < (*c_arg).argumentCount; index++) {
   argv[index + 1] = (*c_arg).arguments[index];
  }
  argv[index + 1] = NULL;

  if (pid == 0) {
    int status = execvpe((*c_arg).command, argv, envp);
    if (status != 0) {
      fputs(strerror(errno), stdout);
    }
    exit(0);
  } else {
      do {
        waitpid(pid, &waitStatus, WUNTRACED);
      } while (!WIFEXITED(waitStatus) && !WIFSIGNALED(waitStatus));
  }
}


void executeBinary(commandArgument *c_arg) {
  if ((*c_arg).argumentCount == 0) {
    executeBinaryInteractively(c_arg);
  } else {
    if(*(*c_arg).arguments[(*c_arg).argumentCount - 1] == '&') {
      // execute in background
    } else {
      executeBinaryInteractively(c_arg);
    }
  }
}


void executeBuiltInPwd(commandArgument *c_arg) {
 if ((*c_arg).argumentCount > 1) {
  fputs("No argument is allowed for pwd.", stdout);
  fputs("\n", stdout);
  return;
 }
  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
   fputs(cwd, stdout);
   fputs("\n", stdout);
  }
}

void executeBuiltInCd(commandArgument *c_arg ) {
 if ((*c_arg).argumentCount > 1) {
  fputs("Max 1 argument is allowed for cd.", stdout);
  fputs("\n", stdout);
  return;
 } else if ((*c_arg).argumentCount == 1) {
   int result = chdir((*c_arg).arguments[0]);
   if (result != 0) {
     fputs(strerror(errno), stdout);
     fputs("\n", stdout);
    }
  }  
  else {
     int result = chdir("");
     if (result != 0) {
       fputs(strerror(errno), stdout);
       fputs("\n", stdout);
     }
 }
}

int getBuiltInCode(commandArgument * c_arg) {
 if (strcmp("cd", (*c_arg).command) == 0) {
  return (int) BUILT_IN_CD;
 } else if (strcmp("pwd", (*c_arg).command) == 0) {
  return (int) BUILT_IN_PWD;
 }
 return 0;
}


void executeCommand(commandArgument * c_arg) {
  int builtInCode = getBuiltInCode(c_arg);
  if(builtInCode) {
    switch (builtInCode) {
      case 1 :
              executeBuiltInCd(c_arg);
              break;
      case 2 :
              executeBuiltInPwd(c_arg);
              break;
      default:
           fputs("BuiltIn is not implemented", stdout);
           fputs("\n", stdout);
    }
  } else {
     executeBinary(c_arg);
  }
}


void printParsedCommand(commandArgument * c_Arg) {
  if (c_Arg != NULL) {
    fputs("command : ", stdout);
    fputs((*c_Arg).command, stdout);
    fputs("\n", stdout);
    for (int i = 0; i < (*c_Arg).argumentCount; i++) {
       fputs("Argument ", stdout);
       fputs(" : ", stdout);
       fputs((*c_Arg).arguments[i], stdout);
       fputs("\n", stdout);
    }
  } else {
   fputs("parsed command is NULL", stdout);
   fputs("\n", stdout);
  }

}


int main(int argc, char *argv[], char *envp[]) {
 
  char buffer[BUFFER_SIZE];
  while (TRUE) {
   char *input;
   fputs("sbush > ", stdout);
   input = fgets(buffer, BUFFER_SIZE, stdin);

   commandArgument *c_Arg = parseInput(input, ' ');
   #ifdef DEBUG
     printParsedCommand(c_Arg);
   #endif

   executeCommand(c_Arg);

   if (input == NULL || *input == '\n') {
     fputs("No command found. Exiting", stdout);
     fputs("\n", stdout);
     return 1;
   }

   if (!strcmp(input, "exit\n"))	{
     fputs("Exiting as requested", stdout);
     fputs("\n", stdout);
     return 1;
    }
  }
  return 0;
}
