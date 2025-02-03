#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/kernel.h>

#define Sct 0xffffffff8a000320  // Replace with actual address

typedef void (*function)(void);
typedef unsigned char int8;
typedef unsigned short int int16;
typedef unsigned int int32;
typedef unsigned long long int int64;

static function *syscalltable;
static void *mcount = 0;
asmlinkage int (*origkill)(pid_t, int);

static int __init setup(void);
static void __exit cleanup(void);
asmlinkage int newkill(pid_t, int);
static void remove_protection(void);
static void restore_protection(void);

static void printhex(int8 *str, int16 size, int8 delim) {
    int8 *p;
    int16 n;
    int8 buf[512];

    for(n=0; n<size;n++) 
        *(buf+n)=0;

    for(p=str, n=size; n; n--, p++) {
        sprintf(buf, "%s.02x",buf, *p);
        if(delim)
            sprintf("%c",delim);

    }
    pr_info("%s",buf);

    return;

}

static void remove_protection() {
    asm volatile("push %rax");
    asm volatile("push %rbx");
    asm volatile("mov %cr0,%rax");
    asm volatile("mov $0xfffeffff,%rbx");
    asm volatile("and %rbx,%rax");
    asm volatile("mov %rax,%cr0");
    asm volatile("pop %rbx");
    asm volatile("pop %rax");

    return;
}

static void restore_protection() {
    asm volatile("mov %cr0, %rax\n");
    asm volatile("mov $0x00010000, %rbx\n");
    asm volatile("or %rbx, %rax\n");
    asm volatile("mov %rax, %cr0\n");
    asm volatile("pop %rbx");
    asm volatile("pop %rax");

}

asmlinkage int newkill(pid_t _, int __) {
    return -EPERM;
}

static int __init setup(void) {
    unsigned int size;
    void *mem, *mem2;

    size = 37*4;
    mem = (unsigned int *)Sct;
    syscalltable = (function*)mem;

    mem2 = *(syscalltable+size);
    origkill = mem2;
    printhex((int8 *)origkill,64,'x');

    remove_protection();
    //*(syscalltable+size) = (function)&newkill;
    restore_protection();

    pr_info("sct.o successfully loaded!\n");
    return 0;
}

static void __exit cleanup(void) {
    unsigned int size;

    size = 37*4;
    //remove_protection();
    //*(syscalltable+size) = (function)origkill;
    //restore_protection();
    //
    pr_info("sct.o successfully unloaded!\n");
    return;
}

module_init(setup);
module_exit(cleanup);

MODULE_LICENSE("GPL");

