#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "syscall.h"


void execute_cat(char * arg) {
	puts("\n in cat \n");
	puts(arg);
}

void execute_ls() {

}

void execute_cd(char * arg) {
	puts("\n in cd \n");
	puts(arg);
}

void extract_arg(char * start, char * arg) {
	char * trav = start;
    int no_of_arguments = 0;
	while(trav != NULL && *trav != '\0' && no_of_arguments <= 1) {
		if (*trav != ' ') {
			*arg++ = *trav++;
		} else {
			no_of_arguments += 1;
			trav++;
		}
	}
	*arg = '\0';
 }


int parse_command(char * buff, char * arg) {
	if (buff == NULL || *buff == '\0') {
		return -1;
	}
	if (strlen(buff) >= 2 && buff[0] == 'c' && buff[1] == 'd') {
		extract_arg(buff + 2, arg);
		return 1;
	}

	if (strlen(buff) >= 2 && buff[0] == 'l' && buff[1] == 's') {
         return 2;
    }

	if (strlen(buff) >= 3 && buff[0] == 'c' && buff[1] == 'a' && buff[2] == 't') {
		 extract_arg(buff + 3, arg);
         return 3;
	}
	return -1;
}

int main(int argc, char *argv[], char *envp[]) {
    
    char buf[100];
	char arg[100];
    while(1) {
      puts("\nsbush~>");
      gets(buf);
      puts(buf);
      switch(parse_command(buf, arg)) {
		case 1:
			// cd <>
			execute_cd(arg);
			break;
		case 2:
			// ls
			execute_ls();
			break;
		case 3:
			// cat <>
			execute_cat(arg);
			break;
		default:
			puts("\nCommand Not Found");
	    
	  }
	  memset(arg, '\0', strlen(arg));
      memset(buf, '\0', strlen(buf)); 
    }

    return 0;
}

