#
# kern_process.s
#

.text


.global switch_to
switch_to:
    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rbp
    pushq %rdi
    pushq %rsi
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
    movq  %rsp,%rax
    movq  %rax,0x8(%rdi)
    movq  0x8(%rsi),%rax
    movq  %rax,%rsp
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rsi
    popq %rdi
    popq %rbp
    popq %rdx
    popq %rcx
    popq %rbx
    popq %rax
    retq
    retq

.global function_2

.global first_switch_to
first_switch_to:
    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rbp
    pushq %rdi
    pushq %rsi
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
    movq  %rsp,%rax
    movq  %rax,0x8(%rdi)
    movq  0x8(%rsi),%rax
    movq  %rax,%rsp
    call function_2
    retq

#.global test_user_function
#test_user_function:
#    int $0x80
#    iretq
