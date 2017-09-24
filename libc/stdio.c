#include <stdio.h>
#include <sys/types.h>

FILE in  = {0};
FILE out = {1};
FILE err = {2};

FILE* STDIN  = &in;
FILE* STDOUT = &out;
FILE* STDERR = &err;


