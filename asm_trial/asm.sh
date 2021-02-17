nasm -f elf64 sample3.asm && \
    ld -s -o a.out sample3.o && \
    objdump -d -M intel sample3.o && \
    ./a.out
