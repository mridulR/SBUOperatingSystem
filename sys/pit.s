#
# idt.s
#

.text

.global pit_interrupt_service_routine
pit_interrupt_service_routine:
  push %rbp
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
  call helper_calculate_timer 
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

