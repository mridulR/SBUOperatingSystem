#include <stdio.h>
#include <string.h>
#define ENVIRON_DATA_SIZE 20

typedef struct environ_data {
  char * key;
  char *value;
} ENVIRON_DATA;


 void * malloc(size_t size);
extern ENVIRON_DATA *environ_vars[ENVIRON_DATA_SIZE];

ENVIRON_DATA *environ_vars[] = {};


char * getenv(char * key) {
 for(int index = 0; index < ENVIRON_DATA_SIZE; index++) {
   if (environ_vars[index] == NULL) {
     return NULL;
   } else if (strcmp(key, environ_vars[index]->key) == 0) {
     return environ_vars[index]->value;
   }
 }
 return NULL;
}

int setenv(char * key, char * value, int mode) {
  if (mode == 0) {  // only add, don't overwrite
    for (int index = 0; index < ENVIRON_DATA_SIZE; index++) {
      if(environ_vars[index] == NULL) {
        environ_vars[index] = (ENVIRON_DATA *) malloc(sizeof(ENVIRON_DATA));
        environ_vars[index]->key = key;
        environ_vars[index]->value = value;
        return 0;
      }		
    }
    // space full
    return -1;
  } else if (mode == 1) {
    for (int index = 0; index < ENVIRON_DATA_SIZE; index++) {
      if (environ_vars[index] == NULL) {
        environ_vars[index] = (ENVIRON_DATA *)malloc(sizeof(ENVIRON_DATA));
        environ_vars[index]->key = key;
        environ_vars[index]->value = value;
        return 0;
      } else {
        if (strcmp(environ_vars[index]->key, key) == 0) {
          environ_vars[index]->value = value;
          return 0;
        }
      }
    }
    // space full
    return -1; 
  } else {
   // mode not supported
   return -1;
  } 
}
