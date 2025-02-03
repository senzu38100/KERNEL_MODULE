bits 64
global _start

_start:
    mov rcx,0x0000000080050033
    mov rax,rcx
    mov rbx,0x00010000
    sub rbx,0xffffffff
    and rax,rbx
    mov rcx,rax

    asm volatile(mov %cr0,%rax);
    asm volatile(mov $0xffeffff,%rbx);
    asm volatile(and %rbx,%rax);
    asm volatile(mov%rax,%cr0);

    mov rdi, 1   ; First argument (stdout for sys_write)
    mov rsi, rcx ; Second argument (buffer/message)
    mov rdx, 14  ; Third argument (length of message)

    mov rax, 1   ; syscall number for sys_write
    syscall      ; Call kernel
