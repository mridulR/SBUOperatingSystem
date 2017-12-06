#include <sys/syscall.h>

int read(int fd, void *buf, int size)
{
    uint64_t ret;
    uint64_t syscall_num = (uint64_t)__NR_read_64;
    uint64_t arg1 = (uint64_t)fd;
    uint64_t arg2 = (uint64_t)buf;
    uint64_t arg3 = (uint64_t)size;
    __asm__ __volatile__
    (
        "movq %1,%%rsi\n"
        "movq %2,%%rdi\n"
        "movq %3,%%rbx\n"
        "movq %4,%%rcx\n"
        "int $0x80;\n"
        "movq %%rax,%0\n"
        : "=r"(ret)
        : "r"(syscall_num) , "r"(arg1), "r"(arg2), "r"(arg3)
        : "cc", "memory"
    );
    return (int)ret;
}

int write(int fd, const void *buf, int size)
{
    uint64_t ret;
    uint64_t syscall_num = (uint64_t)__NR_write_64;
    uint64_t arg1 = (uint64_t)fd;
    uint64_t arg2 = (uint64_t)buf;
    uint64_t arg3 = (uint64_t)size;
    __asm__ __volatile__
    (
        "movq %1,%%rsi\n"
        "movq %2,%%rdi\n"
        "movq %3,%%rbx\n"
        "movq %4,%%rcx\n"
        "int $0x80;\n"
        "movq %%rax,%0\n"
        : "=r" (ret)
        : "r"(syscall_num), "r"(arg1), "r"(arg2), "r"(arg3)
        : "cc", "memory"
    );
    return (int)ret;
}

/*int getpid()
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

