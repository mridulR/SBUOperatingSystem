#include<string.h>

int strcmp(const char * s1, const char * s2) {
  while ((s1 != NULL && s2 != NULL) && ((*s1 != '\0') && (*s2 != '\0')) && (*s1 == *s2)) {
    s1++;
    s2++;
  }
  if (s1 == NULL || s2 == NULL) {
    return -1;
  }
  return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

