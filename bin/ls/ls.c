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
    //TODO: FIX opendir, readdir first buddy
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
    return 1;
}
