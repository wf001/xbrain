bits 64
global _start

_start:
    mov rsi, rsp
    push 0x10
    pop rax
    not rax
    mov r13, 0x15
    and rax,r13
    push rax

    mov rax, 0x3c
    syscall

