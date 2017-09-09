#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/syscall.h>

int main(int argc, char* argv[], char* env[])
{
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
    }
    closedir(current_working_directory);*/
    return 1;
}
