#include <string.h>

int strlen(const char *str){
    int len = 0;
    if(!str){
        return len;
    }
    while(*++str != '\0'){
        len++;
    }
    return len;
}
