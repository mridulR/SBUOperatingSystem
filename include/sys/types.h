#ifndef _TYPES_H
#define _TYPES_H

#define NULL ((void*)0)
#define EOF (-1)
#define __exit __section(.exit.text) __exitused __cold notrace

#define EXIT_SUCCESS 0 
#define EXIT_FAILURE -1
#define SA_NOCLDWAIT  0x00000002



typedef unsigned long  uint64_t;
typedef          long   int64_t;
typedef unsigned int   uint32_t;
typedef          int    int32_t;
typedef unsigned short uint16_t;
typedef          short  int16_t;
typedef unsigned char   uint8_t;
typedef          char    int8_t;

typedef uint64_t size_t;
typedef int64_t ssize_t;

typedef uint64_t off_t;

typedef uint32_t pid_t;

typedef unsigned short mode_t;

typedef int16_t     intptr_t;

struct __sFile
{
  int fd;
};

typedef struct __sFile FILE;

extern int fileno(FILE*);

typedef uint32_t __uid_t;
typedef uint32_t __uid_t;

#endif
