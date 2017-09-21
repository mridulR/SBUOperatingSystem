#ifndef _ASM_UTIL_H
#define _ASM_UTIL_H

static inline void pushad() {
__asm__ __volatile__
  (
  "push %rax\n"
  "push %rbx\n"
  "push %rcx\n"
  "push %rdx\n"
  "push %rdi\n"
  "push %rsi\n"
  "push %r10\n"
  "push %r11\n"
  "push %r12\n"
  "push %r13\n"
  "push %r14\n"
  "push %r15\n"
  );
}


static inline void popad() {
  __asm__ __volatile__
  (
    "pop %rax\n" 
    "pop %rbx\n"
    "pop %rcx\n"
    "pop %rdx\n"
    "pop %rdi\n"
    "pop %rsi\n"
    "pop %r10\n"
    "pop %r11\n"
    "pop %r12\n"
    "pop %r13\n"
    "pop %r14\n"
    "pop %r15\n"
    "pop %rbp\n"
    "iretq\n"
  );
}

#endif
