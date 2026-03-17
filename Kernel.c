// kernel.c
// This is a real kernel that can be loaded by a bootloader
#include <stdint.h>

void kernel_main() {
    // Video memory starts at 0xb8000 for text mode
    volatile uint16_t* video_memory = (uint16_t*)0xb8000;
    
    const char *message = "Hello, this is a real kernel!";
    uint16_t color = (0x0F << 8); // white on black

    for (int i = 0; message[i] != '\0'; i++) {
        video_memory[i] = color | message[i];
    }

    while (1); // hang so it stays on screen
}
