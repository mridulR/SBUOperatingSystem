#include "syscall.h"

int clrscr() {

	uint64_t ret = 0;

	uint64_t syscall_num = (uint64_t)__NR_clrscr_64;
 
      __asm__ __volatile__
      (
      "movq %1,%%rax\n"
      "int $0x80\n"
      "movq %%rax,%0\n"
      : "=g" (ret)
      : "g"(syscall_num)
      : "rax"
      );
      
      return (int)ret;

}

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
	: "=g" (ret)
	: "g"(syscall_num) , "g"(arg1), "g"(arg2), "g"(arg3)
    : "rax", "rbx", "rcx", "rdx"
	);

    return (int)ret;
}


int write(int fd, const void *buf, int size) {

    uint64_t ret;
    uint64_t syscall_num = (uint64_t)__NR_write_64;
    uint64_t arg1 = (uint64_t)fd;
    uint64_t addr = (uint64_t )buf;
    uint64_t arg2 = addr;
    uint64_t arg3 = (uint64_t)size;

	__asm__ __volatile__
	(
	"movq %1,%%rax\n"
	"movq %2,%%rbx\n"
	"movq %3,%%rcx\n"
	"movq %4,%%rdx\n"
	"int $0x80\n"
	"movq %%rax,%0\n"
	: "=g" (ret)
	: "g"(syscall_num), "g"(arg1), "g"(arg2), "g"(arg3)
    : "rax", "rbx", "rcx", "rdx"
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
	: "=g" (pid)
	: "g"(syscall_num)
    : "rax"
	);  
  return (int) pid;
}


void *mmap(void *addr, int length, int prot, int flags, int fd, int offset)
{
   uint64_t syscall_num = (uint64_t)__NR_mmap_64;
   uint64_t arg2 = (uint64_t) length;
   uint64_t arg3 = (uint64_t) prot;
   uint64_t arg4 = (uint64_t) flags;
  
   uint64_t ret_val = 0;
  
      __asm__ __volatile__
      (
      "movq %1,%%rax\n"
      "movq %2,%%rbx\n"
      "movq %3,%%rcx\n"
      "movq %4,%%rdx\n"
      "int $0x80\n"
      "movq %%rax,%0\n"
      : "=g" (ret_val)
      : "g"(syscall_num) , "g"(arg2), "g"(arg3), "g"(arg4)
      : "rax", "rbx", "rcx", "rdx"
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
      "movq %3,%%rcx\n"
      "int $0x80\n"
      "movq %%rax,%0\n"
      : "=g" (ret_val)
      : "g"(syscall_num) , "g"(arg1), "g"(arg2)
      : "rax", "rbx", "rcx", "rdx"
    );

 return (int)ret_val;
}



int open(const char *path, int permissions)
{
    uint64_t syscall_num = (uint64_t)__NR_open_64;
    uint64_t arg1 = (uint64_t) path;
    uint64_t arg2 = (uint64_t) permissions;
        
    uint64_t ret_val = 0;

    __asm__ __volatile__
    (
        "movq %1,%%rax\n"
        "movq %2,%%rbx\n"
        "movq %3,%%rcx\n"
        "int $0x80\n"
        "movq %%rax,%0\n"
        : "=g" (ret_val)
        : "g"(syscall_num) , "g"(arg1), "g"(arg2)
        : "rax", "rbx", "rcx"
    );

    return (int)ret_val;
}

int close(int fd)
{
    uint64_t syscall_num = (uint64_t)__NR_close_64;
    uint64_t arg1 = (uint64_t) fd;
        
    uint64_t ret_val = 0;

    __asm__ __volatile__
    (
        "movq %1,%%rax\n"
        "movq %2,%%rbx\n"
        "int $0x80\n"
        "movq %%rax,%0\n"
        : "=g" (ret_val)
        : "g"(syscall_num) , "g"(arg1)
        : "rax", "rbx", "rcx"
    );
    return (int)ret_val;
}

void exit(int status)
{
    uint64_t syscall_num = (uint64_t)__NR_exit_64;
    uint64_t arg1 = (uint64_t) status;
    uint64_t ret_val = 0;

    __asm__ __volatile__
    (
        "movq %1,%%rax\n"
        "movq %2,%%rbx\n"
        "int $0x80\n"
        "movq %%rax,%0\n"
        : "=g" (ret_val)
        : "g"(syscall_num), "g"(arg1)
        : "rax"
    );
}

int pipe(int pipefd[2])
{
    uint64_t syscall_num = (uint64_t)__NR_pipe_64;
    uint64_t arg1 = (uint64_t)pipefd[0];
    uint64_t arg2 = (uint64_t)pipefd[1];
    uint64_t ret_val = 0;

  __asm__ __volatile__
  (
      "movq %1,%%rax\n"
      "movq %2,%%rbx\n"
      "movq %3,%%rcx\n"
      "int $0x80\n"
      "movq %%rax,%0\n"
      : "=g" (ret_val)
      : "g"(syscall_num), "g"(arg1), "g"(arg2)
      : "rax", "rbx", "rcx"
  );
  return (int)ret_val;
}

int dup2(int oldfd, int newfd)
{
  uint64_t syscall_num = (uint64_t)__NR_dup2_64;
  uint64_t arg1 = (uint64_t) oldfd;
  uint64_t arg2 = (uint64_t) newfd;
  uint64_t ret_val = 0;

  __asm__ __volatile__
  (
      "movq %1,%%rax\n"
      "movq %2,%%rbx\n"
      "movq %3,%%rcx\n"
      "int $0x80\n"
      "movq %%rax,%0\n"
      : "=g" (ret_val)
      : "g"(syscall_num), "g"(arg1), "g"(arg2)
      : "rax", "rbx", "rcx"
  );
  return (int)ret_val;
}

int fork()
{
  uint64_t syscall_num = (uint64_t)__NR_fork_64;
  uint64_t ret_val = 0;

  __asm__ __volatile__
  (
      "movq %1,%%rax\n"
      "int $0x80\n"
      "movq %%rax,%0\n"
      : "=g" (ret_val)
      : "g"(syscall_num)
      : "rax"
  );
  return (int)ret_val;
}

int execve(const char *path, char *const argv[], char *const envp[])
{

  uint64_t syscall_num = (uint64_t)__NR_execve_64;
  uint64_t arg1 = (uint64_t) path;
  uint64_t arg2 = (uint64_t) argv;
  uint64_t arg3 = (uint64_t) envp;
  uint64_t ret_val = 0;

  __asm__ __volatile__
  (
      "movq %1,%%rax\n"
      "movq %2,%%rbx\n"
      "movq %3,%%rcx\n"
      "movq %4,%%rdx\n"
      "int $0x80\n"
      "movq %%rax,%0\n"
      : "=g" (ret_val)
      : "g"(syscall_num), "g" (arg1), "g" (arg2), "g"(arg3)
      : "rax", "rbx", "rcx", "rdx"
  );
  return (int)ret_val;
}

int waitpid(int pid, int *status, int options)
{

  uint64_t syscall_num = (uint64_t)__NR_waitpid_64;
  uint64_t arg1 = (uint64_t) pid;
  uint64_t arg2 = (uint64_t) status;
  uint64_t arg3 = (uint64_t) options;
  uint64_t ret_val = 0;

  __asm__ __volatile__
  (
      "movq %1,%%rax\n"
      "movq %2,%%rbx\n"
      "movq %3,%%rcx\n"
      "movq %4,%%rdx\n"
      "int $0x80\n"
      "movq %%rax,%0\n"
      : "=g" (ret_val)
      : "g"(syscall_num), "g" (arg1), "g" (arg2), "g"(arg3)
      : "rax", "rbx", "rcx", "rdx"
  );
  return (int)ret_val;
}

int chdir(const char*path)
{

  uint64_t syscall_num = (uint64_t)__NR_chdir_64;
  uint64_t arg1 = (uint64_t) path;
  uint64_t ret_val = 0;

  __asm__ __volatile__
  (
      "movq %1,%%rax\n"
      "movq %2,%%rbx\n"
      "int $0x80\n"
      "movq %%rax,%0\n"
      : "=g" (ret_val)
      : "g"(syscall_num), "g" (arg1)
      : "rax", "rbx"
  );
  return (int)ret_val;
}

char *getcwd(char *buf, int size)
{
  uint64_t syscall_num = (uint64_t)__NR_getcwd_64;
  uint64_t arg1 = (uint64_t) buf;
  uint64_t arg2 = (uint64_t) size;
  uint64_t ret_val = 0;

  __asm__ __volatile__
  (
      "movq %1,%%rax\n"
      "movq %2,%%rbx\n"
      "movq %3,%%rcx\n"
      "int $0x80\n"
      "movq %%rax,%0\n"
      : "=g" (ret_val)
      : "g"(syscall_num), "g" (arg1), "g" (arg2)
      : "rax", "rbx", "rcx"
  );

  if(ret_val == 0)
    return buf;

  return '\0';
}

int setpgid(int pid, int pgid)
{

  uint64_t syscall_num = (uint64_t)__NR_setpgid_64;
  uint64_t arg1 = (uint64_t) pid;
  uint64_t arg2 = (uint64_t) pgid;
  uint64_t ret_val = 0;

  __asm__ __volatile__
  (
      "movq %1,%%rax\n"
      "movq %2,%%rbx\n"
      "movq %3,%%rcx\n"
      "int $0x80\n"
      "movq %%rax,%0\n"
      : "=g" (ret_val)
      : "g"(syscall_num), "g" (arg1), "g" (arg2)
      : "rax", "rbx", "rcx"
  );

  return ret_val;
}

int getpgid(int pid)
{
  uint64_t syscall_num = (uint64_t)__NR_getpgid_64;
  uint64_t arg1 = (uint64_t) pid;
  uint64_t ret_val = 0;

  __asm__ __volatile__
  (
      "movq %1,%%rax\n"
      "movq %2,%%rbx\n"
      "int $0x80\n"
      "movq %%rax,%0\n"
      : "=g" (ret_val)
      : "g"(syscall_num), "g" (arg1)
      : "rax", "rbx"
  );

  return ret_val;
}

int getpgrp(int pid)
{

  uint64_t syscall_num = (uint64_t)__NR_getpgrp_64;
  uint64_t arg1 = (uint64_t) pid;
  uint64_t ret_val = 0;

  __asm__ __volatile__
  (
      "movq %1,%%rax\n"
      "movq %2,%%rbx\n"
      "int $0x80\n"
      "movq %%rax,%0\n"
      : "=g" (ret_val)
      : "g"(syscall_num), "g" (arg1)
      : "rax", "rbx"
  );
  
  return ret_val;
}

off_t lseek(int fd, off_t offset, int whence)
{
  uint64_t syscall_num = __NR_lseek_64;
  uint64_t arg1 = (uint64_t) fd;
  uint64_t arg2 = (uint64_t) offset;
  uint64_t arg3 = (uint64_t) whence;
  uint64_t ret_val;

  __asm__ __volatile__
  (
      "movq %1,%%rax\n"
      "movq %2,%%rbx\n"
      "movq %3,%%rcx\n"
      "movq %4,%%rdx\n"
      "int $0x80\n"
      "movq %%rax,%0\n"
      : "=g" (ret_val)
      : "g"(syscall_num), "g" (arg1), "g"(arg2), "g" (arg3)
      : "rax", "rbx", "rcx", "rdx"
  );
  return ret_val;
}


dir_info * opendir(char *name) {
	uint64_t syscall_num =  __NR_opendir_64;
	uint64_t arg1 = (uint64_t) name; 
    uint64_t ret_val;

  __asm__ __volatile__
  (
      "movq %1,%%rax\n"
      "movq %2,%%rbx\n"
      "int $0x80\n"
      "movq %%rax,%0\n"
      : "=g" (ret_val)
      : "g"(syscall_num), "g" (arg1)
      : "rax", "rbx"
  );

  return (dir_info *)ret_val;
}

struct dirent *readdir(dir_info *dirp) { 
    uint64_t syscall_num = __NR_readdir_64;
    uint64_t arg1 = (uint64_t) dirp;
    uint64_t ret_val;

  __asm__ __volatile__
  (
      "movq %1,%%rax\n"
      "movq %2,%%rbx\n"
      "int $0x80\n"
      "movq %%rax,%0\n"
      : "=g" (ret_val)
      : "g"(syscall_num), "g" (arg1)
      : "rax", "rbx"
  );  

  return (dirent *)ret_val;
}

int closedir(dir_info *dirp) { 
    uint64_t syscall_num = __NR_closedir_64;
    uint64_t arg1 = (uint64_t) dirp;
    uint64_t ret_val;

  __asm__ __volatile__
  (
      "movq %1,%%rax\n"
      "movq %2,%%rbx\n"
      "int $0x80\n"
      "movq %%rax,%0\n"
      : "=g" (ret_val)
      : "g"(syscall_num), "g" (arg1)
      : "rax", "rbx"
  );  

  return (int)ret_val;
}


