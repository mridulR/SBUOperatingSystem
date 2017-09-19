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
  push %r10
  push %r11
  push %r12
  push %r13
  push %r14
  push %r15
  push %rbp
  call helper_keyboard_handler
  pop %rax 
  pop %rcx
  pop %rdx
  pop %rbx
  pop %rsi
  pop %rdi
  pop %r10
  pop %r11
  pop %r12
  pop %r13
  pop %r14
  pop %r15
  pop %rbp
  iretq
  addq $8, %rsp
