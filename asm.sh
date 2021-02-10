nasm -f elf64 sample2.asm && \
    ld -s -o a.out sample2.o && \
    objdump -d -M intel sample2.o && \
    ./a.out
