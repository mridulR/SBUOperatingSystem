#
# idt.s
#

.text

.global keyboard_interrupt_service_routine
keyboard_interrupt_service_routine:
  push %rax
  push %rcx
  push %rdx
  push %rbx
  push %rsi
  push %rdi
  push %r8
  push %r9
  push %r10
  push %r11
  push %r12
  push %r13
  push %r14
  push %r15
  push %rbp
  call helper_keyboard_handler 
  pop %rbp
  pop %r15
  pop %r14
  pop %r13
  pop %r12
  pop %r11
  pop %r10
  pop %r9
  pop %r8
  pop %rdi
  pop %rsi
  pop %rbx
  pop %rdx
  pop %rcx
  pop %rax
  iretq
