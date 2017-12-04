#include<sys/kstring.h>
#include<sys/kprintf.h>

int kstrcmp(const char *s1, const char *s2) {
    if (*s1 != *s2) {
        return (*(const unsigned char *)s1 - *(const unsigned char *)s2);
    }   

    while(*s1 == *s2) {
        s1++;
        s2++;
        if (*s1 != *s2) {
            return (*(const unsigned char *)s1 - *(const unsigned char *)s2);
        }
        if (*s1 == '\0' && *s2 == '\0') {
            return 0;
        }
    }
	return -1;   
}

int kstrlen(char * str) {
	int len = 0;
    char *ptr = str;
	while (*ptr++ != '\0') {
		len++;
    }
	return len;
}


char * kstr_tok(char * str, const char delimeter, char ** remaining) {
	if (str == NULL || *str == '\0') {
        return NULL;
    }
	char *trav = str;
	char *begin = str;
	if (*str == '\0') {
		return str;
	}
	while (*trav != delimeter && *trav != '\0') {
		trav++;
    }
	if (kstrlen(trav) != 0) {
		*trav = '\0';
        *remaining = trav + 1;
	} else {
		*remaining = '\0';
	}
    return begin;
}



