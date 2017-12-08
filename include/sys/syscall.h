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


#define __NR_opendir_64 250
#define __NR_readdir_64 251
#define __NR_closedir_64 252
#define __NR_clrscr_64 255

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

#define MAP_ANONYMOUS 0x20
#define MAP_PRIVATE 0x02
#define MAP_SHARED 0x01
#define PROT_READ 0x1
#define PROT_WRITE 0x2
#define PROT_EXEC 0x4
#define PROT_NONE 0x0

#endif
