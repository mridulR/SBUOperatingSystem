#ifndef _DEFS_H
#define _DEFS_H

#define NULL ((void*)0)
int EOF = -1;

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

typedef uint32_t __uid_t;
typedef uint32_t __uid_t;

int fileno (FILE *fp)
{
  return (fp->fd);
}


#define _NIOBRW 16
extern FILE _iob[_NIOBRW];

#define stdin   (&_iob[0])
#define stdout  (&_iob[1])
#define stderr  (&_iob[2])
#endif
