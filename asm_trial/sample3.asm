bits 64
global _start

_start:
    mov rsi, rsp
    push 0x41C8
    pop rax
    mov r15, rax
    and r15, 0xff
    sar rax,8
    and rax,r15
    push rax

    mov rax, 0x3c
    syscall

