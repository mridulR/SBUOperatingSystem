#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/syscall.h>

#define O_NONBLOCK  0x0004
#define O_RDONLY    0x0000
#define O_CLOEXEC   0x00100000
#define O_DIRECTORY 0x00020000

int main(int argc, char* argv[], char* env[])
{
    char cwd[1024];
    //char buf[200];
    getcwd(cwd, sizeof(cwd));
    int fd = open(cwd, O_RDONLY|O_NONBLOCK|O_DIRECTORY|O_CLOEXEC);
    if(fd == -1){
      fputs("Unable to open current directory\n", stdout);
    }
    //TODO: Loop aropund to print line by line 
    /*DIR *current_working_directory;
    struct dirent *file;
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    current_working_directory = opendir(cwd);
    while((file = readdir(current_working_directory)) != NULL)
    {
        if ((strcmp(file->d_name, ".") != 0) && (strcmp(file->d_name,"..") != 0)) {
          fputs(file->d_name, stdout);
          fputs("\n", stdout);
        }
    }*/
    close(fd);
    return 1;
}
