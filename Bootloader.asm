[org 0x7c00]
bits 16

start:
    ; load kernel (assume it’s right after boot sector)
    cli                 ; disable interrupts
    mov ax, 0x1000      ; load address
    mov ds, ax
    jmp 0x1000          ; jump to kernel_main (in real kernel, we’d load here)

times 510-($-$$) db 0
dw 0xaa55
