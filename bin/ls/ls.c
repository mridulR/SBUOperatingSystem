#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <dirent.h>

void execute_ls(char * arg) {
    if (arg == NULL || *arg == '\0') {
		char buf[40];
		memset(buf, '\0', 40);
		getcwd(buf, 40);
		memcpy(arg, buf, strlen(buf));
	}
	dir_info* dir = opendir(arg);
	if (dir != NULL) {
		struct dirent *dir_entry =  readdir(dir);
		while (dir_entry != NULL) {
			//printf("\n%s", dir_entry->d_name);
			dir_entry =  readdir(dir);	
		}
		closedir(dir);
	}
}

int main(int argc, char *argv[], char *envp[]) {
    execute_ls(argv[0]);
    return 0;
}

