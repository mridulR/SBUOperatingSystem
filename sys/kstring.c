#include<sys/kstring.h>
#include<sys/kprintf.h>


void k_concat(char * str1, char * str2) {
 if (str1 == NULL || str2 == NULL) {
	return;
  }
  int i = 0;
  int j = 0;
  for (; str1[i] != '\0'; ++i) {

  }
 
  for (; str2[j] != '\0'; ++j) {
	str1[i + j] = str2[j];
  }

  str1[i + j] = '\0';
}


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

int kstrlen(const char * str) {
	if (str == NULL || *str == '\0') {
		return 0;
	}
	return (*str) ? kstrlen(++str) + 1 : 0;
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



