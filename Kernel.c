#include <stdint.h>
#include <stddef.h>

// ================= VGA =================
volatile uint16_t* vga = (uint16_t*)0xB8000;
int cursor = 0;

void putc(char c) {
    if (c == '\n') {
        cursor += (80 - (cursor % 80));
        return;
    }
    vga[cursor++] = (uint16_t)c | (0x0F << 8);
}

void print(const char* s) {
    while (*s) putc(*s++);
}

// ================= PORT IO =================
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0,%1"::"a"(val),"Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t r;
    __asm__ volatile ("inb %1,%0":"=a"(r):"Nd"(port));
    return r;
}

// ================= IDT =================
struct IDTEntry {
    uint16_t off_low;
    uint16_t sel;
    uint8_t zero;
    uint8_t flags;
    uint16_t off_high;
} __attribute__((packed));

struct IDTPtr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct IDTEntry idt[256];
struct IDTPtr idtp;

extern void idt_load();

void idt_set(int n, uint32_t handler) {
    idt[n].off_low = handler & 0xFFFF;
    idt[n].sel = 0x08;
    idt[n].zero = 0;
    idt[n].flags = 0x8E;
    idt[n].off_high = handler >> 16;
}

// ================= PIC =================
void pic_remap() {
    outb(0x20,0x11);
    outb(0xA0,0x11);
    outb(0x21,0x20);
    outb(0xA1,0x28);
    outb(0x21,0x04);
    outb(0xA1,0x02);
    outb(0x21,0x01);
    outb(0xA1,0x01);
    outb(0x21,0);
    outb(0xA1,0);
}

// ================= KEYBOARD =================
char keymap[128] = {
0,27,'1','2','3','4','5','6','7','8','9','0','-','=',8,9,
'q','w','e','r','t','y','u','i','o','p','[',']','\n',0,'a','s',
'd','f','g','h','j','k','l',';',39,'`',0,'\\','z','x','c','v',
'b','n','m',',','.','/',0,'*',0,' '
};

__attribute__((interrupt))
void keyboard_handler(void* frame) {
    uint8_t sc = inb(0x60);
    if (sc < 128) {
        char c = keymap[sc];
        if (c) putc(c);
    }
    outb(0x20,0x20);
}

// ================= PAGING =================
uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t first_page_table[1024] __attribute__((aligned(4096)));

void paging_init() {
    for (int i = 0; i < 1024; i++) {
        first_page_table[i] = (i * 0x1000) | 3; // present + rw
    }

    for (int i = 0; i < 1024; i++) {
        page_directory[i] = 0;
    }

    page_directory[0] = (uint32_t)first_page_table | 3;

    __asm__ volatile("mov %0, %%cr3"::"r"(page_directory));
    uint32_t cr0;
    __asm__ volatile("mov %%cr0, %0":"=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ volatile("mov %0, %%cr0"::"r"(cr0));
}

// ================= HEAP (VERY BASIC) =================
uint32_t heap_top = 0x1000000;

void* kmalloc(size_t size) {
    void* r = (void*)heap_top;
    heap_top += size;
    return r;
}

// ================= INIT =================
extern void keyboard_stub();

void idt_init() {
    idtp.limit = sizeof(idt)-1;
    idtp.base = (uint32_t)&idt;

    for(int i=0;i<256;i++) idt_set(i,0);

    idt_set(33,(uint32_t)keyboard_stub);

    idt_load();
}

// ================= KERNEL =================
void kernel_main() {
    print("Kernel start\n");

    paging_init();
    print("Paging on\n");

    pic_remap();
    idt_init();

    __asm__ volatile("sti");

    print("Interrupts on\n");
    print("Type: ");

    while (1) {
        __asm__ volatile("hlt");
    }
}
