bits 64
section .text
global _start

_start:
    mov rax, 0x72
    and rax, 0x24
    add rax, 0x18
    push rax
    mov rsi, rsp
    mov rdi, 0x1
    mov rax, 0x1
    mov rdx, 0x1
    syscall
    mov rax, 0x3c
    syscall
