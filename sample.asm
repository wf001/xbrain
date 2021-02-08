bits 64
section .text
global _start

_start:
    mov rax, 0x4C
    and rax, 0x2E
    mov r11, rax
    jmp .loop

    
.loop:
    cmp rax, 0x0
    jz .print
    and r11, 0xf
    cmp r11, 0xa
    jae .a
    jb .b

.a:
    add r11, 0x37
    jmp .c
.b:
    add r11, 0x30
    jmp .c

.c
    push r11
    tar rax, 4
    mov r11, rax
    inc r12
    jmp .loop

.print:
    mov rsi, rsp
    mov rdi, 0x1
    mov rax, 0x1
    mov rdx, 0x1
    syscall
    pop r13

    dec r12
    cmp r12, 0x0
    jz .hlt
    jmp .print

.hlt:
    mov rax, 0x3c
    syscall
