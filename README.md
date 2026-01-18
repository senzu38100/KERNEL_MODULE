# KERNEL_MODULE

# README - Unkillable Process Kernel Module

##  Project Overview
This project demonstrates how to create an **"immortal process"** in Linux by preventing it from being killed. Normally, you can terminate a process using the `kill` command:
kill <PID>

Even if the process handles signals (signal() function in C), it can still be forcibly terminated using:
kill -9 <PID>

which is almost impossible to intercept at the user level. However, by modifying the 
Linux Kernel System Call Table (SCT), we can intercept the kill() system call and prevent specific processes from being terminated.


!!!!!!!!!!!!!!!! I RECOMMAND YOU TO DO IT ON A VM, YOU WILL AVOID A LOT OF ISSUES IF THINGS GOES WRONG !!!!!!!!!!!!!!!


How It Works
System Call Hooking
The kill() system call is intercepted by modifying the system call table (sys_call_table).
If the process being killed is the "protected" process, the kernel denies the request and returns an error (-EPERM).
2. Disabling Kernel Write Protection
The Linux kernel has built-in memory protections (CR0 register), which prevent modifying read-only kernel structures.
The module disables kernel write protection, modifies the system call table, then re-enables protection.
3. Finding the System Call Table Address
Older kernels exported sys_call_table, but modern kernels use KASLR (Kernel Address Space Layout Randomization) to prevent direct access.
The module retrieves the address from:
/boot/System.map-$(uname -r) (from debug packages)
/proc/kallsyms (if accessible)
Installation & Usage
1. Requirements
Linux kernel 6.1.0-29-amd64 (this module was tested on this version).
Kernel headers installed:
sudo apt install linux-headers-$(uname -r)
NASM for assembling .asm files:
sudo apt install nasm

2. Compilation
To compile the kernel module, run:
make

If Secure Boot is enabled, sign the kernel module:
sudo mokutil --import MOK.der
Then reboot and enroll the key.

3. Load the Module
sudo insmod sct5.ko

Check if it was loaded successfully:
dmesg | tail -20

4. Test the Protection
Run a test process:
./some_process &
Try killing it:
kill -9 <PID>
If successful, the kernel should block the termination.

5. Unload the Module

sudo rmmod sct5

Confirm it was removed:
lsmod | grep sct5

Research & Implementation Notes
The Linux system call table is not exported in modern kernels, requiring debugging techniques to locate its address.
The kernel CR0 register (write-protected bit) must be modified to allow writing to sys_call_table.
Potential Risks:
Directly modifying kernel structures is considered unsafe.
The module could cause system instability or crashes if misused.
Ensure testing is done in a virtual machine (VM) or non-critical system.
References
The Linux Kernel Module Programming Guide
https://sysprog21.github.io/lkmpg/
Syscall Hooking Guide by Bruno Ciccarino
https://github.com/BrunoCiccarino/LinuxLowlevelAddict
Legal Disclaimer
This project is intended for research purposes only.
Modifying system calls can introduce security risks and should only be done with proper authorization.
Use this module only on systems you own or have permission to modify.
