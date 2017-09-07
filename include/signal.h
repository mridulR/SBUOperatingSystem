#ifndef _SIGNAL_H
#define _SIGNAL_H

#include <sys/defs.h>

#define	_WSTOPPED 0177		/* _WSTATUS if process is stopped */
#define	_W_INT(w) (*(int *)&(w))	/* convert union wait to int */
#define	_WSTATUS(x) (_W_INT(x) & 0177)
#define WIFEXITED(x) (_WSTATUS(x) == 0)
#define WIFSIGNALED(x)	(_WSTATUS(x) != _WSTOPPED && _WSTATUS(x) != 0)

#define SIG_DFL     (void (*)(int))0
#define SIG_IGN     (void (*)(int))1
#define SIG_ERR     (void (*)(int))-1
#define	SIGCHLD	20	/* to parent on child stop or exit */
typedef unsigned int sigset_t;

int kill(pid_t pid, int sig);

// OPTIONAL: implement for ``signals and pipes (+10 pts)''
typedef void (*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler);

typedef union sigval {
  int sival_int;
  void *sival_ptr;
} sigval_t;

typedef struct siginfo
  {
    int si_signo;                /* Signal number.  */
    int si_errno;                /* If non-zero, an errno value associated with
                                   this signal, as defined in <errno.h>.  */
    int si_code;                /* Signal code.  */
    int  si_pid;                /* Sending process ID.  */
    int  si_uid;                /* Real user ID of sending process.  */
    void *si_addr;                /* Address of faulting instruction.  */
    int si_status;                /* Exit value or signal.  */
    long int si_band;                /* Band event for SIGPOLL.  */
    sigval_t si_value;                /* Signal value.  */
} siginfo_t;

typedef struct sigaction_data  {
  void (*sa_handler)(int);
  void (*sa_sigaction)(int, siginfo_t *, void *);
  sigset_t sa_mask;
  int sa_flags;
  void (*sa_restorer)(void);
} SigactionData;


int sigaction(int signum, const SigactionData *act, SigactionData *oldact);

#endif
