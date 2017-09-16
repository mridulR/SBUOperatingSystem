#
# idt.s
#

.text

.global default_interrupt_service_routine
default_interrupt_service_routine:
  push %rax
  push %rcx
  push %rdx
  push %rbx
  push %rbp
  push %rsi
  push %rdi
  push %r10
  push %r11
  push %r12
  push %r13
  push %r14
  push %r15
  mov $0x20, %al
  mov $0x20, %dx
  out %al, %dx
  pop %rax 
  pop %rcx
  pop %rdx
  pop %rbx
  pop %rbp
  pop %rsi
  pop %rdi
  pop %r10
  pop %r11
  pop %r12
  pop %r13
  pop %r14
  pop %r15
  retq

