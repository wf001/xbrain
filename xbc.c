#define _DEFAULT_SOURCE  // fileno
#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>

#define MEMORY_SIZE 30000
//#define DEBUG

const char *program_name = "xbc";
#define FATAL(message)                                      \
    do {                                                    \
        fprintf(stderr, "%s: %s\n", program_name, message); \
        exit(EXIT_FAILURE);                                 \
    } while (0)

#ifdef DEBUG
#define _info(x, f) printf("### " #x " = %" #f "\n", (x));
#define _sep() printf("%s", "==========================\n");

#else
#define _info(x, f) 1 ? (void)0 : printf
#define _sep() 1 ? (void)0 : printf

#endif

#define PROGRAM_INIT \
    { 0, 0, NULL, 0, 0, NULL }
#define PROGRAM \
    (struct program) { 0 }

enum ins {
    INS_MOVE,
    INS_MUTATE,
    INS_IN,
    INS_OUT,
    INS_BRANCH,
    INS_JUMP,
    INS_HALT,
};

enum mode { MODE_OPEN, MODE_FUNCTION, MODE_STANDALONE };

struct program {
    size_t max, count;
    struct {
        enum ins ins;
        long operand;
    } * ins;
    size_t markers_max, markers_count;
    long *markers;
};
struct asmbuf {
    size_t size, fill;
    uint8_t code[];
};

void mark_program(struct program *);
long unmark_program(struct program *);
void add_program(struct program *, enum ins, long);
void free_program(struct program *);
void parse_program(struct program *, FILE *);

void free_program(struct program *p) {
    free(p->ins);
    free(p->markers);
}

void add_program(struct program *p, enum ins ins, long operand) {
    if (p->count == p->max) {
        if (p->max == 0)
            p->max = 256;
        else
            p->max *= 2;
        size_t size = sizeof(p->ins[0]) * p->max;
        p->ins = realloc(p->ins, size);
    }
    switch (ins) {
        case INS_BRANCH:
            mark_program(p);
            break;
        case INS_JUMP: {
            long sibling = unmark_program(p);
            if (sibling < 0) FATAL("unmatched ']'");
            p->ins[sibling].operand = p->count + 1;
            operand = sibling;
        } break;
        case INS_MOVE:
        case INS_MUTATE:
        case INS_IN:
        case INS_OUT:
        case INS_HALT:
            break;
    }
    p->ins[p->count].ins = ins;
    p->ins[p->count].operand = operand;
    p->count++;
    _sep();
    _info(p->ins[p->count].ins, d);
    _info(p->ins[p->count].operand, d);
    _info(p->count, d);
}

void mark_program(struct program *p) {
    if (p->markers_count == p->markers_max) {
        if (p->markers_max == 0)
            p->markers_max = 16;
        else
            p->markers_max *= 2;
        size_t size = sizeof(p->markers[0]) * p->markers_max;
        p->markers = realloc(p->markers, size);
    }
    p->markers[p->markers_count++] = p->count;
}

long unmark_program(struct program *p) {
    if (p->markers_count > 0)
        return p->markers[--p->markers_count];
    else
        return -1;
}

void parse_program(struct program *p, FILE *in) {
    int c;
    while ((c = fgetc(in)) != EOF) {
        switch (c) {
            case '+':
                add_program(p, INS_MUTATE, 1);
                break;
            case '-':
                add_program(p, INS_MUTATE, -1);
                break;
            case '>':
                add_program(p, INS_MOVE, 1);
                break;
            case '<':
                add_program(p, INS_MOVE, -1);
                break;
            case '.':
                add_program(p, INS_OUT, 0);
                break;
            case ',':
                add_program(p, INS_IN, 0);
                break;
            case '[':
                add_program(p, INS_BRANCH, 0);
                break;
            case ']':
                add_program(p, INS_JUMP, 0);
                break;
            default:
                /* Nothing */
                break;
        }
    }
    if (p->markers_count > 0) FATAL("unmatched '['");
    add_program(p, INS_HALT, 0);
}

struct asmbuf *create_asmbuf(void);
void free_asmbuf(struct asmbuf *);
void finalize_asmbuf(struct asmbuf *);
void add_asmbuf_ins(struct asmbuf *, int, uint64_t);
void add_asmbuf_immediate(struct asmbuf *, int, const void *);
void add_asmbuf_syscall(struct asmbuf *, int);

struct asmbuf *create_asmbuf(void) {
    long page_size = sysconf(_SC_PAGESIZE);
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_ANONYMOUS | MAP_PRIVATE;
    size_t size = page_size * 1024;
    struct asmbuf *buf = mmap(NULL, size, prot, flags, -1, 0);
    buf->size = size;
    return buf;
}

void free_asmbuf(struct asmbuf *buf) { munmap(buf, buf->size); }

void finalize_asmbuf(struct asmbuf *buf) {
    mprotect(buf, buf->size, PROT_READ | PROT_EXEC);
}

void add_asmbuf_ins(struct asmbuf *buf, int size, uint64_t ins) {
    for (int i = size - 1; i >= 0; i--)
        buf->code[buf->fill++] = (ins >> (i * 8)) & 0xff;
}

void add_asmbuf_immediate(struct asmbuf *buf, int size, const void *value) {
    memcpy(buf->code + buf->fill, value, size);
    buf->fill += size;
}

void add_asmbuf_syscall(struct asmbuf *buf, int syscall) {
    if (syscall == 0) {
        add_asmbuf_ins(buf, 2, 0x31C0);  // xor  eax, eax
    } else if (syscall == 1) {
        add_asmbuf_ins(buf, 3, 0x4C89E0);  // mov rax, r12
    } else {
        add_asmbuf_ins(buf, 1, 0xB8);  // mov  rax, syscall
        uint32_t n = syscall;
        add_asmbuf_immediate(buf, 4, &n);
    }
    add_asmbuf_ins(buf, 2, 0x0F05);  // syscall
}

struct asmbuf *compile(const struct program *, enum mode);

struct asmbuf *compile(const struct program *program, enum mode mode) {
    uint32_t memory_size = MEMORY_SIZE;
    struct asmbuf *buf = create_asmbuf();

    /* Clear zero register */
    if (mode == MODE_FUNCTION) {
        add_asmbuf_ins(buf, 1, 0x53);    // push rbx
        add_asmbuf_ins(buf, 2, 0x4154);  // push r12
    }
    add_asmbuf_ins(buf, 2, 0x31dB);          // xor ebx, ebx
    add_asmbuf_ins(buf, 6, 0x41BC01000000);  // mov r12, 1

    /* Allocate BF array on stack */
    add_asmbuf_ins(buf, 3, 0x4881EC);  // sub  rsp, X
    add_asmbuf_immediate(buf, 4, &memory_size);
    add_asmbuf_ins(buf, 3, 0x4889E6);      // mov  rsi, rsp
    add_asmbuf_ins(buf, 5, 0xBA01000000);  // mov  edx, 0x1

    /* Clear BF array */
    add_asmbuf_ins(buf, 2, 0x30C0);    // xor  al, al
    add_asmbuf_ins(buf, 3, 0x4889E7);  // mov  rdi, rsp
    add_asmbuf_ins(buf, 1, 0xB9);      // mov  rcx, X
    add_asmbuf_immediate(buf, 4, &memory_size);
    add_asmbuf_ins(buf, 2, 0xF3AA);  // rep stosb

    /* rsi - data pointer
     * rdi - syscall argument
     * rax - temp and store return value, syscall number
     * rbx - zero register
     * r12 - one register
     * r13 - temp
     * r14 - counter
     *
     */
    uint32_t *table = malloc(sizeof(table[0]) * program->count);
    for (size_t i = 0; i < program->count; i++) {
        enum ins ins = program->ins[i].ins;
        long operand = program->ins[i].operand;
        table[i] = buf->fill;
        switch (ins) {
            case INS_MOVE:
                if (operand > -256 && operand < 256) {
                    if (operand > 0) {
                        add_asmbuf_ins(buf, 3, 0x4883C6);  // add  rsi, byte X
                    } else {
                        operand *= -1;
                        add_asmbuf_ins(buf, 3, 0x4883EE);  // sub  rsi, byte X
                    }
                    add_asmbuf_immediate(buf, 1, &operand);
                } else {
                    if (operand > 0) {
                        add_asmbuf_ins(buf, 3, 0x4881C6);  // add  rsi, X
                    } else {
                        operand *= -1;
                        add_asmbuf_ins(buf, 3, 0x4881EE);  // sub  rsi, X
                    }
                    add_asmbuf_immediate(buf, 4, &operand);
                }
                break;
            case INS_MUTATE:
                if (operand > 0) {
                    add_asmbuf_ins(buf, 2, 0x8006);  // add  byte [rsi], X
                } else {
                    operand *= -1;
                    add_asmbuf_ins(buf, 2, 0x802E);  // sub  byte [rsi], X
                }
                add_asmbuf_immediate(buf, 1, &operand);
                break;
            case INS_IN:
                add_asmbuf_ins(buf, 3, 0x4831FF);  // xor  rdi, rdi
                add_asmbuf_syscall(buf, SYS_read);
                break;
            case INS_OUT:
                add_asmbuf_ins(buf, 3, 0x4C89E7);  // mov  rdi, r12
                add_asmbuf_syscall(buf, SYS_write);
                break;
            case INS_BRANCH: {
                uint32_t delta = 0;
                add_asmbuf_ins(buf, 2, 0x381E);        // cmp  [rsi], bl
                add_asmbuf_ins(buf, 2, 0x0F84);        // jz
                add_asmbuf_immediate(buf, 4, &delta);  // patched by JUMP ']'
            } break;
            case INS_JUMP: {
                uint32_t delta = table[operand];
                delta -= buf->fill + 5;
                add_asmbuf_ins(buf, 1, 0xE9);  // jmp delta
                add_asmbuf_immediate(buf, 4, &delta);
                void *jz = &buf->code[table[operand] + 4];
                uint32_t patch = buf->fill - table[operand] - 8;
                memcpy(jz, &patch, 4);  // patch previous branch '['
            } break;
            case INS_HALT:
                if (mode == MODE_FUNCTION) {
                    add_asmbuf_ins(buf, 3, 0x4881C4);  // add  rsp, X
                    add_asmbuf_immediate(buf, 4, &memory_size);
                    add_asmbuf_ins(buf, 2, 0x415C);  // pop r12
                    add_asmbuf_ins(buf, 1, 0x5B);    // pop rbx
                    add_asmbuf_ins(buf, 1, 0xC3);    // ret
                } else if (mode == MODE_STANDALONE) {
                    add_asmbuf_ins(buf, 3, 0x4831FF);  // xor  rdi, rdi
                    add_asmbuf_syscall(buf, SYS_exit);
                }
                break;
        }
    }
    free(table);

    finalize_asmbuf(buf);
    return buf;
}

void write_elf(struct asmbuf *buf, FILE *elf) {
    uint64_t entry = 0x400000 + sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr);
    Elf64_Ehdr ehdr = {
        .e_ident =
            {
                ELFMAG0,
                ELFMAG1,
                ELFMAG2,
                ELFMAG3,
                ELFCLASS64,
                ELFDATA2LSB,
                EV_CURRENT,
                ELFOSABI_SYSV,
            },
        .e_type = ET_EXEC,
        .e_machine = EM_X86_64,
        .e_version = EV_CURRENT,
        .e_entry = entry,
        .e_phoff = sizeof(Elf64_Ehdr),
        .e_ehsize = sizeof(Elf64_Ehdr),
        .e_phentsize = sizeof(Elf64_Phdr),
        .e_phnum = 1,
    };
    Elf64_Phdr phdr = {
        .p_type = PT_LOAD,
        .p_flags = PF_X | PF_R,
        .p_offset = sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr),
        .p_vaddr = entry,
        .p_filesz = buf->fill,
        .p_memsz = buf->fill,
        .p_align = 0,
    };

    fwrite(&ehdr, sizeof(ehdr), 1, elf);
    fwrite(&phdr, sizeof(phdr), 1, elf);
    fwrite(buf->code, buf->fill, 1, elf);
}

void print_help(const char *argv0, FILE *o) {
    fprintf(o, "Usage: %s [-o <file>] [-h] \n", argv0);
    fprintf(o, "  -o <file>    executable output file name\n");
    fprintf(o, "  -h           print this usage information\n");
}

int main(int argc, char **argv) {
    /* Options */
    program_name = argv[0];
    char *output = NULL;
    char *input = NULL;

    /* Parse arguments */
    int option;
    while ((option = getopt(argc, argv, "o:eiDhO:")) != -1) {
        switch (option) {
            case 'o':
                output = optarg;
                break;
            default:
                print_help(argv[0], stderr);
                FATAL("invalid option");
        }
    }
    if (optind >= argc)
        FATAL("no input files");
    else if (optind != argc - 1)
        FATAL("too many input files");
    input = argv[optind];

    char output_buf[1024];
    if (output == NULL) {
        snprintf(output_buf, sizeof(output_buf), "%s", input);
        output = output_buf;
        char *p = output + strlen(output);
        while (*p != '.' && p >= output) p--;
        if (p < output)
            FATAL("no output file specified");
        else
            *p = '\0';
    }

    struct program program = PROGRAM_INIT;
    FILE *source = fopen(argv[optind], "r");
    if (source == NULL) FATAL("could not open input file");
    // PARSE
    parse_program(&program, source);
    fclose(source);

    // COMPILE
    struct asmbuf *buf = compile(&program, MODE_STANDALONE);
    // BUILD
    FILE *elf = fopen(output, "wb");
    if (elf == NULL) FATAL("could not open output file");
    write_elf(buf, elf);
    fchmod(fileno(elf), 0755);
    fclose(elf);
    free_asmbuf(buf);

    free_program(&program);
    return 0;
}