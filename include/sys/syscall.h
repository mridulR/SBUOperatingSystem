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
#define __NR_munmap_64 11 
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
#define __NR_brk_64 12 
#define __NR_lseek_64 8

int read(int fd, void *buf, int size) {

    uint64_t ret = 0;
    uint64_t syscall_num = (uint64_t)__NR_read_64;
    uint64_t arg1 = (uint64_t)fd;
    uint64_t arg2 = (uint64_t)buf;
    uint64_t arg3 = (uint64_t)size;

	 __asm__ __volatile__
	(
	"movq %1,%%rax\n"
	"movq %2,%%rbx\n"
	"movq %3,%%rcx\n"
	"movq %4,%%rdx\n"
	"int $0x80\n"
	"movq %%rax,%0\n"
	: "=r" (ret)
	: "g"(syscall_num) , "g"(arg1), "g"(arg2), "g"(arg3)
    : "rax", "memory"
	);

    return (int)ret;
}


int write(int fd, const void *buf, int size) {

    uint64_t ret;
    uint64_t syscall_num = (uint64_t)__NR_write_64;
    uint64_t arg1 = (uint64_t)fd;
    uint64_t addr = (uint64_t )buf;
    uint64_t arg2 = addr;
    //uint64_t arg2 = (uint64_t)0xf0000fee;
    uint64_t arg3 = (uint64_t)size;

	__asm__ __volatile__
	(
	"movq %1,%%rax\n"
	"movq %2,%%rbx\n"
	"movq %3,%%rcx\n"
	"movq %4,%%rdx\n"
	"int $0x80\n"
	"movq %%rax,%0\n"
	: "=r" (ret)
	: "g"(syscall_num), "g"(arg1), "g"(arg2), "g"(arg3)
    : "rax", "memory"
	);

    return (int)ret;
}


int getpid()
{

	uint64_t syscall_num = (uint64_t)__NR_getpid_64;
	 uint64_t pid = 0;
	__asm__ __volatile__
	(
	"movq %1,%%rax\n"
	"int $0x80\n"
	"movq %%rax,%0\n"
	: "=r" (pid)
	: "g"(syscall_num)
    : "cc"
	);  
  return (int) pid;
}

void *mmap(void *addr, int length, int prot, int flags, int fd, int offset)
{
   uint64_t syscall_num = (uint64_t)__NR_mmap_64;
   uint64_t arg1 = 0;
   uint64_t arg2 = (uint64_t) length;
   uint64_t arg3 = (uint64_t) prot;
   uint64_t arg4 = (uint64_t) flags;
   uint64_t arg5 = (uint64_t) fd;
   uint64_t arg6 = (uint64_t) offset;
  
   uint64_t ret_val = 0;
  
      __asm__ __volatile__
      (
      "movq %1,%%rax\n"
      "movq %2,%%rbx\n"
      "movq %3,%%rcx\n"
      "movq %4,%%rdx\n"
      "movq %5,%%r8\n"
      "movq %6,%%r9\n"
      "movq %7,%%r10\n"
      "int $0x80\n"
      "movq %%rax,%0\n"
      : "=r" (ret_val)
      : "g"(syscall_num) , "g"(arg1), "g"(arg2), "g"(arg3), "g"(arg4), "g"(arg5), "g"(arg6)
      : "cc"
      );

   return (void *)ret_val;
}
 
int munmap(void *addr, int length)
{
 uint64_t syscall_num = (uint64_t)__NR_munmap_64;
  
        uint64_t arg1 = (uint64_t) addr;
        uint64_t arg2 = (uint64_t) length;
        
        uint64_t ret_val = 0;
   
       __asm__ __volatile__
        (
        "movq %1,%%rax\n"
        "movq %2,%%rbx\n"
        "int $0x80\n"
        "movq %%rax,%0\n"
        : "=r" (ret_val)
        : "g"(syscall_num) , "g"(arg1), "g"(arg2)
        : "cc"
        );

 return (int)ret_val;
}


/*
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

void *mmap(void *addr, int length, int prot, int flags, int fd, int offset)
{
  void* ret;
  __asm__ __volatile__
  (
      "syscall"
      :"=a" (ret)
      :"0"(__NR_mmap_64), "b" (addr), "c" (length), "d" (prot), "g" (flags), "g" (fd), "g" (offset)
      :"cc"
  );
  return ret;
}

int munmap(void *addr, int length)
{
  int ret;
  __asm__ __volatile__
  (
      "syscall"
      :"=a" (ret)
      :"0"(__NR_munmap_64), "b" (addr), "c" (length)
      :"cc"
  );
  return ret;
}

off_t lseek(int fd, off_t offset, int whence)
{
  long int ret;
  __asm__ __volatile__
  (
      "syscall"
      :"=a" (ret)
      :"0"(__NR_lseek_64), "b" (fd), "c" (offset), "d" (whence)
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

int execvpe(const char *file, char *const argv[], char *const envp[])
{
  fputs(envp[0], stdout);
  int retVal = execve(file, argv, envp);
  return retVal;
}*/


#endif
