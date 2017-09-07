#ifndef __SYSCALL_H
#define __SYSCALL_H

#include <unistd.h>
#include <stdlib.h>

#define O_RDONLY    0x0000      /* open for reading only */
#define O_WRONLY    0x0001      /* open for writing only */
#define O_RDWR      0x0002      /* open for reading and writing */
#define O_ACCMODE   0x0003      /* mask for above modes */
#define WNOHANG     1           /* dont hang in wait */
#define WUNTRACED   2           /* tell about stopped, untraced children */
#define TIOCGPGRP   0x540F
#define TIOCSPGRP   0x5410

#define __NR_read_64 0
#define __NR_write_64 1
#define __NR_getpid_64 39
#define __NR_open_64 2 
#define __NR_close_64 3 
#define __NR_exit_64 60 
#define __NR_mmap_64 9 
#define __NR_brk_64 12 
#define __NR_pipe_64 22 
#define __NR_dup2_64 33
#define __NR_fork_64 57 
#define __NR_execve_64 59 
#define __NR_waitpid_64 61
#define __NR_getcwd_64 79 
#define __NR_chdir_64 80 
#define __NR_getpgid_64 121
#define __NR_setpgid_64 109
#define __NR_getpgrp_64 111
#define __NR_ioctl_64 16


int read(int fd, void *buf, int size)
{
    int ret;
    __asm__ __volatile__
    (
        "syscall"
        : "=a" (ret)
        : "0"(__NR_read_64), "b"(fd), "c"(buf), "d"(size)
        : "cc", "memory"
    );
    return ret;
}

int write(int fd, const void *buf, int size)
{
    int ret;
    __asm__ __volatile__
    (
        "syscall"
        : "=a" (ret)
        : "0"(__NR_write_64), "D"(fd), "S"(buf), "d"(size)
        : "cc", "rcx", "r11", "memory"
    );
    return ret;
}

int getpid()
{
  int ret;
  __asm__ __volatile__
  (
      "syscall"
      : "=a" (ret)
      : "0"(__NR_getpid_64)
      : "cc", "rcx", "r11"
  );
  return ret;
}

int open(const char *path, int permissions)
{
    int fd;
    __asm__ __volatile__
    (
        "syscall"
        : "=a" (fd)
        : "0"(__NR_open_64), "b"(path), "c"(permissions)
        : "cc"
    );
    return fd;
}

int close(int fd)
{
    int ret;
    __asm__ __volatile__
    (
        "syscall"
        : "=a" (ret)
        : "0"(__NR_close_64), "b"(fd)
        : "cc"
    );
    return ret;
}

void exit()
{
  int ret;
  __asm__ __volatile__
  (
      "syscall"
      :"=a" (ret)
      :"0"(__NR_exit_64)
      :"cc"
  );
}

int pipe(int pipefd[2])
{
  int ret;
  __asm__ __volatile__
  (
      "syscall"
      :"=a" (ret)
      :"0"(__NR_pipe_64), "b" (pipefd[0]), "c" (pipefd[1])
      :"cc"
  );
  return ret;
}

int dup2(int oldfd, int newfd)
{
  int ret;
  __asm__ __volatile__
  (
      "syscall"
      :"=a" (ret)
      :"0"(__NR_dup2_64), "b" (oldfd), "c" (newfd)
      :"cc"
  );
  return ret;
}

int fork()
{
  int ret;
  __asm__ __volatile__
  (
      "syscall"
      :"=a" (ret)
      :"0"(__NR_fork_64)
      :"cc"
  );
  return ret;
}

int execve(const char *path, char *const argv[], char *const envp[])
{
  int ret;
  __asm__ __volatile__
  (
      "syscall"
      :"=a" (ret)
      :"0"(__NR_execve_64), "b" (path), "c" (argv), "d" (envp)
      :"cc"
  );
  return ret;
}

int waitpid(int pid, int *status, int options)
{
  int ret;
  __asm__ __volatile__
  (
      "syscall"
      :"=a" (ret)
      :"0"(__NR_waitpid_64), "b" (pid), "c" (status), "d" (options)
      :"cc"
  );
  return ret;
}

int chdir(const char*path)
{
  int ret;
  __asm__ __volatile__
  (
      "syscall"
      :"=a" (ret)
      :"0"(__NR_chdir_64), "b" (path)
      :"cc"
  );
  return ret;
}

char *getcwd(char *buf, int size)
{
  int ret;
  __asm__ __volatile__
  (
      "syscall"
      :"=a" (ret)
      :"0"(__NR_getcwd_64), "b" (buf), "c" (size)
      :"cc"
  );
  if(ret == 0)
    return buf;
  return '\0';
}

int setpgid(int pid, int pgid)
{
  int ret;
  __asm__ __volatile__
  (
      "syscall"
      :"=a" (ret)
      :"0"(__NR_setpgid_64), "b" (pid), "c" (pgid)
      :"cc"
  );
  return ret;
}

int getpgid(int pid)
{
  int ret;
  __asm__ __volatile__
  (
      "syscall"
      :"=a" (ret)
      :"0"(__NR_getpgid_64), "b" (pid)
      :"cc"
  );
  return ret;
}

int getpgrp(int pid)
{
  int ret;
  __asm__ __volatile__
  (
      "syscall"
      :"=a" (ret)
      :"0"(__NR_getpgrp_64), "b" (pid)
      :"cc"
  );
  return ret;
}

// TODO: Currently it returns if the current process is foregraound or not
int tcgetpgrp(int fd)
{
   int ret;
   int val;
  __asm__ __volatile__
  (
      "syscall"
      :"=a" (ret)
      :"0"(__NR_ioctl_64), "b" (fd), "c" (TIOCGPGRP), "d" (&val)
      :"cc"
  );
  return ret;
}

int tcsetpgrp(int fd, int pgrp)
{
  int ret;
  __asm__ __volatile__
  (
      "syscall"
      :"=a" (ret)
      :"0"(__NR_ioctl_64), "b" (fd), "c" (TIOCSPGRP), "d" (&pgrp)
      :"cc"
  );
  return ret;
}

#if 0
#define __NR_write_32 4
int write(int fd, const void *buf, int size)
{
    int ret;
    __asm__ __volatile__
    (
        "int $0x80"
        : "=a" (ret)
        : "0"(__NR_write_32), "b"(fd), "c"(buf), "d"(size)
        : "cc", "edi", "esi", "memory"
    );
    return ret;
}

#define __NR_read_32 3
int read(int fd, const void *buf, int size)
{
    int ret;
    __asm__ __volatile__
    (
        "int $0x80"
        : "=a" (ret)
        : "0"(__NR_read_32), "b"(fd), "c"(buf), "d"(size)
        : "cc", "memory"
    );
    return ret;
}

int getpid()
{
__asm__ __volatile__(
        "movl $14,%eax;    \n"\
        "movl %eax,pid;    \n"\
        "int $0x80;        \n");
 return 0;
}

#endif

FILE *fopen(const char *path, const char *mode)
{
  FILE *desc = malloc(sizeof(struct __sFile));
  desc->fd = open(path, *mode);
  return desc;
}


int fclose(FILE *f)
{
  int retVal = close(f->fd);  
  return retVal;
}

#endif
