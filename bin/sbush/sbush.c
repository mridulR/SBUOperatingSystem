#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "parser.h"

#define BUILT_IN_CD 1
#define BUILT_IN_PWD 2
#define BUILT_IN_EXPORT 3 
#define BUILT_IN_ECHO 4 
#define BUILT_IN_WITHOUT_EXPORT 5
#define TRUE 1
#define FALSE 0
//#define DEBUG TRUE
#define BUFFER_SIZE 1000
#define MAX_PS1_LENGTH 100

char PS1[MAX_PS1_LENGTH] = "sbush~>";

int execvpe(const char *file, char *const argv[], char *const envp[]);
pid_t waitpid(pid_t pid, int *stat_loc, int options);
char *secure_getenv(const char *name);
void custom_fputs(char * chr, FILE * out);

void waitForProcessExecution(int pid) { 
      int waitStatus =0;    
      do {
        waitpid(pid, &waitStatus, WUNTRACED);
      } while (!WIFEXITED(waitStatus) && !WIFSIGNALED(waitStatus));
}


void setExecutionArguments(char *envp[], char *argv[], commandArgument *c_arg) {
  char *path = getenv("PATH");
  envp[0] = path;
  envp[1] = NULL;
  argv[0] = (*c_arg).command;
  int index = 0;
  for (; index < (*c_arg).argumentCount; index++) {
   argv[index + 1] = (*c_arg).arguments[index];
  }
  argv[index + 1] = NULL;
}

void executeBinaryInBackGround(commandArgument *c_arg) {
  char *envp[2];
  char *argv[(*c_arg).argumentCount + 2];
  setExecutionArguments(envp, argv, c_arg);
  int pid = fork();
  if (pid == 0) {
    setpgid(0,0);
    int status = execvpe((*c_arg).command, argv, envp);
    if (status != 0) {
      custom_fputs(strerror(errno), stdout);
    }
    exit(0);
  }
}

void executeBinaryInteractively(commandArgument *c_arg) {
  char *envp[2];
  char *argv[(*c_arg).argumentCount + 2];
  setExecutionArguments(envp, argv, c_arg);
  int pid = fork();
  if (pid == 0) {
    int status = execvpe((*c_arg).command, argv, envp);
    if (status != 0) {
      custom_fputs((*c_arg).command, stdout);
      custom_fputs(": command not found.\n", stdout);
    }
    exit(0);
  } else {
      waitForProcessExecution(pid);
  }
}


void executeBinary(commandArgument *c_arg) {
  if ((*c_arg).argumentCount == 0) {
    executeBinaryInteractively(c_arg);
  } else {
    if(*(*c_arg).arguments[(*c_arg).argumentCount - 1] == '&') {
      (*c_arg).argumentCount = (*c_arg).argumentCount - 1;   // Ignoring "&"
      executeBinaryInteractively(c_arg);
    } else {
      executeBinaryInteractively(c_arg);
    }
  }
}


void executeBuiltInPwd(commandArgument *c_arg) {
 if ((*c_arg).argumentCount > 1) {
  custom_fputs("No argument is allowed for pwd.", stdout);
  custom_fputs("\n", stdout);
  return;
 }
  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
   custom_fputs(cwd, stdout);
   custom_fputs("\n", stdout);
  }
}

void executeBuiltInEcho(commandArgument *c_arg)
{
  for(int i = 0; i< (*c_arg).argumentCount; i++)
  {
    char *c_dollar = strchr((*c_arg).arguments[i], '$');
    if(c_dollar)
    {
      char * result = secure_getenv(c_dollar + 1);
      custom_fputs(result, stdout);
      custom_fputs("\n",stdout);
    }
  }
}

void executeBuiltInExport(commandArgument *c_arg)
{
  for(int i = 0; i< (*c_arg).argumentCount; i++)
  {
    char *c_equal = strchr((*c_arg).arguments[i], '=');
    if(c_equal)
    {
      *c_equal = '\0';
      setenv((*c_arg).arguments[i], c_equal + 1, 1);
    }
  }
}


void executeBuiltInWithoutExport(commandArgument *c_arg)
{
  char *c_equal = strchr((*c_arg).command, '=');
  if(c_equal)
  {
    (*c_arg).command[c_equal - (*c_arg).command] = '\0';
    char *value = (*c_arg).command + (c_equal - (*c_arg).command) + 1;
    setenv((*c_arg).command, value, 1);
    if(strcmp((*c_arg).command, "PS1") == 0)
    {
      memset(PS1,'\0', 100);
      memcpy(PS1, value, strlen(value));
    }
  }
  executeBuiltInExport(c_arg);
}

void executeBuiltInCd(commandArgument *c_arg ) {
 if ((*c_arg).argumentCount > 1) {
  custom_fputs("Max 1 argument is allowed for cd.", stdout);
  custom_fputs("\n", stdout);
  return;
 } else if ((*c_arg).argumentCount == 1) {
   int result = chdir((*c_arg).arguments[0]);
   if (result != 0) {
     custom_fputs(strerror(errno), stdout);
     custom_fputs("\n", stdout);
    }
  }  
  else {
     int result = chdir("/");
     if (result != 0) {
       custom_fputs(strerror(errno), stdout);
       custom_fputs("\n", stdout);
     }
 }
}

int getBuiltInCode(commandArgument * c_arg) {
 if (strcmp("cd", (*c_arg).command) == 0) {
  return (int) BUILT_IN_CD;
 } else if (strcmp("pwd", (*c_arg).command) == 0) {
  return (int) BUILT_IN_PWD;
 } else if (strcmp("export", (*c_arg).command) == 0) {
  return (int) BUILT_IN_EXPORT;
 } else if (strcmp("echo", (*c_arg).command) == 0) {
  return (int) BUILT_IN_ECHO;
 } else if (strchr((*c_arg).command, '=') != NULL) {
  return (int) BUILT_IN_WITHOUT_EXPORT;
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
      case 3 :
             executeBuiltInExport(c_arg);
             break;
      case 4 :
             executeBuiltInEcho(c_arg);
             break;
      case 5 :
             executeBuiltInWithoutExport(c_arg);
             break;
      default:
           custom_fputs("BuiltIn is not implemented", stdout);
           custom_fputs("\n", stdout);
    }
  } else {
     executeBinary(c_arg);
  }
}


void printParsedCommand(commandArgument * c_Arg) {
  if (c_Arg != NULL) {
    custom_fputs("command : ", stdout);
    custom_fputs((*c_Arg).command, stdout);
    custom_fputs("\n", stdout);
    for (int i = 0; i < (*c_Arg).argumentCount; i++) {
       custom_fputs("Argument ", stdout);
       custom_fputs(" : ", stdout);
       custom_fputs((*c_Arg).arguments[i], stdout);
       custom_fputs("\n", stdout);
    }
  } else {
   custom_fputs("parsed command is NULL", stdout);
   custom_fputs("\n", stdout);
  }

}

void custom_fputs(char * chr, FILE * out) {
 if (chr != NULL) {
   fputs(chr, out);
 }
}


int main(int argc, char *argv[], char *envp[]) {
 
  char buffer[BUFFER_SIZE];
  while (TRUE) {
   char *input;
   custom_fputs(PS1, stdout);
   input = fgets(buffer, BUFFER_SIZE, stdin);

   commandArgument *c_Arg = parseInput(input, ' ');
   #ifdef DEBUG
     printParsedCommand(c_Arg);
   #endif

   if (input == NULL || *input == '\n') {
     continue;
   }

   if (!strcmp(input, "exit\n"))	{
     custom_fputs("Exiting as requested", stdout);
     custom_fputs("\n", stdout);
     return 1;
    }
   
    executeCommand(c_Arg);
    freeCommandArgument(c_Arg);
  }
  return 0;
}
