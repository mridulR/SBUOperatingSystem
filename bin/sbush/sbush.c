#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "parser.h"
#include <sys/syscall.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

#define BUILT_IN_CD 1
#define BUILT_IN_PWD 2
#define BUILT_IN_EXPORT 3 
#define BUILT_IN_ECHO 4 
#define BUILT_IN_WITHOUT_EXPORT 5
#define TRUE 1
#define true 1
#define FALSE 0
#define false 0
//#define DEBUG TRUE
#define BUFFER_SIZE 1000
#define MAX_PS1_LENGTH 100

char PS1[MAX_PS1_LENGTH] = "sbush~>";

int execvpe(const char *file, char *const argv[], char *const envp[]);
void custom_fputs(char * chr, FILE * out);
int waitpid(int pid, int *status, int options);

static int child = 0;

static void gracefulExit(const char* msg) {
  (child ? exit : exit)(EXIT_FAILURE);
}

void waitForProcessExecution(int pid) { 
      int waitStatus =0;    
      do {
        waitpid(pid, &waitStatus, WUNTRACED);
      } while (!WIFEXITED(waitStatus) && !WIFSIGNALED(waitStatus));
}

void parseAndExecuteCommand(char * input);
 
void executeFile(char * file_path) {
  FILE * fp = fopen(file_path, "r");
  char line[150];
  char * newLine = malloc(150 * sizeof(char));
  memset(newLine, '\0', 150 * sizeof(char));
  char *path = getenv("PATH");
  char *envp[] = {path, NULL};
  int isSbush = 0;
  char * currentLine = fgets(line, sizeof(line), fp);
  currentLine[strlen(currentLine) - 1] = '\0';
  if(strcmp(currentLine, "#!rootfs/bin/sbush\0") == 0){
    isSbush = 1;    
  }
  fclose(fp);
  // Hacky way of reading file with fgets as it was getting random characters in buffer.
  if(line[0] == '#' && line[1] == '!') {
    if(isSbush)
    {
      FILE *fp1 = fopen(file_path, "r");
      currentLine = fgets(line, sizeof(line), fp1);
      while(1) {
        if (currentLine == NULL || strlen(currentLine) == 0 || *currentLine == '\0' || *currentLine == '\n') {
          break;
        }
        memset(line, '\0', sizeof(line));
        currentLine = fgets(line, sizeof(line), fp1);
        if (currentLine == NULL || strlen(currentLine) == 0 || *currentLine == '\0' || *currentLine == '\n') {
          // do nothing
        } else {
           parseAndExecuteCommand(currentLine);
        }
     } 
     fclose(fp1);
    }
    else{
      char *argv[3];
      argv[0] = line + 2;
      argv[1] = file_path;
      argv[2] = NULL;
      int status = execvpe( line + 2, argv, envp);
      if (status != 0) {
        custom_fputs("Error while executing other shell \n", stdout);
      }
    }
  }
}


void setExecutionArguments(char *envp[], char *argv[], commandArgument *c_arg) {
  char *path = getenv("PATH");
  envp[0] = path;
  envp[1] = NULL;
  argv[0] = (*c_arg).command;
  int index = 0;
  for (; index <= (*c_arg).argumentCount; index++) {
   argv[index + 1] = (*c_arg).arguments[index];
  }
  argv[index + 1] = NULL;
  
  if(strcmp((*c_arg).command, "rootfs/bin/sbush") != 0)
  {
    char buf[100];
    memset(buf, '\0', 100);
    memcpy(buf, (*c_arg).command, strlen((*c_arg).command));
    memcpy((*c_arg).command, path, strlen(path));
    memcpy((*c_arg).command + strlen(path), buf, strlen(buf));
    *((*c_arg).command + strlen((*c_arg).command)) = '\0';
  }
  else
  {
    executeFile((*c_arg).arguments[0]);
  }
}

void executeBinaryInBackGround(commandArgument *c_arg) {
  char *envp[2];
  char *argv[(*c_arg).argumentCount + 2];
  setExecutionArguments(envp, argv, c_arg);
  int pid = fork();
  if (pid == 0) {
    setpgid(0,0);
    execvpe((*c_arg).command, argv, envp);
    custom_fputs("Unable to run in bg\n", stdout);
  } 
}

void executeBinaryInteractively(commandArgument *c_arg) {
  char *envp[2];
  char *argv[(*c_arg).argumentCount + 2];
  setExecutionArguments(envp, argv, c_arg);
  // TODO: FIX syscall first then uncomment this
  //int pgid = tcgetpgrp(0);
  int pid = fork();
  if (pid == 0) {
    int status = execvpe(argv[0], argv, envp);
    if (status != 0) {
      custom_fputs("Command not found.\n", stdout);
    }
    exit(0);
  } else {
      waitForProcessExecution(pid);
      // TODO: FIX syscall first then uncomment this
      //tcsetpgrp(0, pgid); 
  }
}


void executeBinary(commandArgument *c_arg) {
  if ((*c_arg).isBackground) {
    executeBinaryInBackGround(c_arg);
  } else {
    executeBinaryInteractively(c_arg);
  }
}


void executeBuiltInPwd(commandArgument *c_arg) {
 if ((*c_arg).argumentCount > 1) {
  custom_fputs("No argument is allowed for pwd.", stdout);
  custom_fputs("\n", stdout);
  return;
 }
 char cwd[1024];
 memset(cwd,'\0', 1024);
 getcwd(cwd, 1024);
 custom_fputs(cwd, stdout);
 custom_fputs("\n", stdout);
}

void executeBuiltInEcho(commandArgument *c_arg)
{
  for(int i = 0; i< (*c_arg).argumentCount; i++)
  {
    char *c_dollar = strchr((*c_arg).arguments[i], '$');
    if(c_dollar)
    {
      char * result = getenv(c_dollar + 1);
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
     custom_fputs("Error in cd", stdout);
     custom_fputs("\n", stdout);
    }
  }  
  else {
     int result = chdir("/");
     if (result != 0) {
       custom_fputs("Error in cd", stdout);
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
 if (chr != '\0') {
   fputs(chr, out);
 }
}


void duplicateFileDescriptorsFromInputToOutput(int old_fd, int new_fd) {
  if (old_fd == new_fd) {
     // do nothing
  } else {
     if (dup2(old_fd, new_fd) != -1) {
       close(old_fd);
     } else {
      custom_fputs("error while executing pipes", stdout);
    }  
  }
}

void runPiping(commandArgument *c_Args[], int numberOfCommands, int currentCommandIndex, int file_desc) {
  if ((currentCommandIndex + 1) <= numberOfCommands) {
    if ((currentCommandIndex + 1) == numberOfCommands) {
      duplicateFileDescriptorsFromInputToOutput(file_desc, 0);
      executeBinary(c_Args[currentCommandIndex]);
      gracefulExit("execvp last");
    } else {
      int fd[2];
      if ( pipe(fd) == -1) {
        custom_fputs("Error while creating pipe", stdout);
      }
      switch(fork()) {
        case -1:
           custom_fputs("Some error occured while forking", stdout);
           custom_fputs("\n", stdout);
           gracefulExit("fork");
        case 0:
           close(fd[0]);
           duplicateFileDescriptorsFromInputToOutput(file_desc, 0);
	       duplicateFileDescriptorsFromInputToOutput(fd[1], 1);
           executeBinary(c_Args[currentCommandIndex]);
           gracefulExit("execvp");
        default:
          // close(file_desc);
           close(fd[1]);
           runPiping(c_Args, numberOfCommands, currentCommandIndex + 1, fd[0]); 
      }
    }
  }
}



int countNumberOfPipes(char* input) {
 int count = 0;
 while(*input) {
  if(*input == '|') {
   count++;
  }
  input++;
 }
 return count;
}

void freeAllParsedCommandArguments(commandArgument *c_Args[], int numOfPipes) {
 for (int index = 0; index <= numOfPipes; index++) {
   if ((c_Args[index]) != NULL) {
     freeCommandArgument(c_Args[index]);
   }
 }
}

int isValidateParsedPipeInput(commandArgument *c_Args[], int numOfPipes) {
 for (int index = 0; index <= numOfPipes; index++) {
   if (strlen((*c_Args[index]).command) == 0) {
     return 0;
   }
 } 
 return 1;
}

void parsePipeCommand(commandArgument *c_Args[], int numOfPipes, char * input) {
   char * prev = input;
   char * next;
   for (int i = 0 ; i <= numOfPipes; i++) {
     next = strchr(prev, '|');
     if (next != NULL) { 
       *next = '\0';
     } 
     c_Args[i] = parseInput(prev, ' ');
     if (next != NULL) {
       prev = next + 1;
     } else {
       next = strchr(prev, '\0');
       prev = next + 1;
     }
   }
}

void parseAndExecuteCommand(char * input) {
     char *ch = strchr(input, '|');
     if(ch) {
       // Implement piping support
       int numOfPipes = countNumberOfPipes(input);
       commandArgument *c_Args[numOfPipes + 1];
       parsePipeCommand(c_Args, numOfPipes, input);
       if (isValidateParsedPipeInput(c_Args, numOfPipes)) {
         //TODO: Let's see how to fix it later on.
         /*if (signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
                gracefulExit("signal");
         }
         // TODO: FIX syscall first then uncomment this
         int pgid = tcgetpgrp(0);*/
         int c_pid = fork();
         switch(c_pid) {
          case 0:
            runPiping(c_Args, numOfPipes + 1, 0, 0);
            exit(0);
          default:
            waitForProcessExecution(c_pid);
            // TODO: FIX syscall first then uncomment this
            //tcsetpgrp(0, pgid); 
         }
        } else {
           custom_fputs("Invalid pipe syntex. Try Again", stdout);
           custom_fputs("\n", stdout);
        }
        freeAllParsedCommandArguments(c_Args, numOfPipes);
     } else {
      commandArgument *c_Arg = parseInput(input, ' ');
      #ifdef DEBUG
        printParsedCommand(c_Arg);
      #endif
      executeCommand(c_Arg);
      freeCommandArgument(c_Arg);
      }
    }

int main(int argc, char *argv[], char *envp[]) {
  for(int i=0; i<1; ++i)
  {
    commandArgument *c_arg = malloc(sizeof(commandArgument)); 
    (*c_arg).trimmedInput = NULL;
    (*c_arg).command = malloc((strlen(*envp+i)* sizeof(char))+ 1);
    memset((*c_arg).command, '\0', strlen(*envp+i)+1);
    memcpy((*c_arg).command, *envp+i, strlen(*envp+i)); 
    (*c_arg).arguments[1] = NULL;
    (*c_arg).isBackground = 0;
    (*c_arg).argumentCount = 0;
    executeCommand(c_arg);
  }
  if (argc == 1) {
    // Run interactively
    char buffer[BUFFER_SIZE];
    while (TRUE) {
      custom_fputs(PS1, stdout);
      memset(buffer, '\0', BUFFER_SIZE);
      char * input = fgets(buffer, BUFFER_SIZE, stdin);
      if (input == NULL || *input == '\n') {
        continue;
      }
      if (!strcmp(input, "exit\n")) {
        custom_fputs("Exiting as requested", stdout);
        custom_fputs("\n", stdout);
        return 1;
      }
      parseAndExecuteCommand(input);
    }
  } 
  else if( argc > 1){
    // Run script  ====>  only supporting sbush <filename> with no space after shabang #!<path_to_executor>
    executeFile(argv[1]);
  }
  return 0;
}

