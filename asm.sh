nasm -f elf64 sample.asm && \
    ld -s -o a.out sample.o && \
    objdump -d -M intel sample.o && \
    ./a.out
