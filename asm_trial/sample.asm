bits 64
global _start
; do AND operation, then output it with hex format.
_start:
    mov rax, 0x77FFAC
    and rax, 0x39333AA
    mov r13, rax
    mov r12, 0x1
    mov rbx, 0x0
    cmp r13, rbx
    jz zero
    jmp loop

zero:
    add r13, 0x30
    push r13
    inc r14
    jmp hex
    
loop:
    cmp rax, rbx
    jz hex
    and r13, 0xf
    cmp r13, 0xa
    jae alfa
    jb num

alfa:
    add r13, 0x37
    jmp save
num:
    add r13, 0x30
    jmp save

save:
    push r13
    sar rax, 4
    mov r13, rax
    inc r14
    jmp loop

hex:
    push 0x78
    push 0x30
    inc r14
    inc r14
    jmp print

print:
    mov rsi, rsp
    mov rdi, r12
    mov rax, r12
    mov rdx, r12
    syscall
    pop r13

    dec r14
    cmp r14, rbx
    jz hlt
    jmp print

hlt:
    mov rax, 0x3c
    syscall
