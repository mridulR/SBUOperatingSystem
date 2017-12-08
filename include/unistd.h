#ifndef _UNISTD_H
#define _UNISTD_H

#include <sys/types.h>

int open(const char *pathname, int flags);
int close(int fd);
int read(int fd, void *buf, int count);
int write(int fd, const void *buf, int count);
int unlink(const char *pathname);

int chdir(const char *path);
char *getcwd(char *buf, int size);

int fork();
int execvpe(const char *file, char *const argv[], char *const envp[]);
int wait(int *status);
int waitpid(int pid, int *status, int options);

unsigned int sleep(unsigned int seconds);

int getpid(void);
int getppid(void);

// OPTIONAL: implement for ``on-disk r/w file system (+10 pts)''
off_t lseek(int fd, off_t offset, int whence);
int mkdir(const char *pathname, mode_t mode);
int unlink(const char *pathname);

// OPTIONAL: implement for ``signals and pipes (+10 pts)''
int pipe(int pipefd[2]);

void *sbrk(void* addr);

FILE *fopen(const char *path, const char *mode);

int fclose(FILE *f);

char *getenv(const char *name);
int setenv(const char *name, const char *value, int overwrite);

void *mmap(void *addr, int length, int prot, int flags, int fd, int offset);
int munmap(void *addr, int length);

#endif
