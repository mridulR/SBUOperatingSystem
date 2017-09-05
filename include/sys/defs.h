#ifndef _DEFS_H
#define _DEFS_H

#define NULL ((void*)0)

typedef unsigned long  size_t;
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
typedef uint32_t mode_t;

typedef uint32_t __pid_t;
typedef uint32_t __uid_t;

typedef struct __file
{
  int handle;
} FILE;

#define EOF -1;

#define _NIOBRW 16
extern FILE _iob[_NIOBRW];

#define stdin   (&_iob[0])
#define stdout  (&_iob[1])
#define stderr  (&_iob[2])

#endif
