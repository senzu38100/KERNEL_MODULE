	-- This code has been written in order to work properly on a specific kernel (6.1.0-26-686-pae), if your kernel--
	--is different it may not work)                                                                                --
	-------------------- General idea --------------------
	in linux, we can stop a processus by killing it: typing the command kill <process id> and it terminates it

	although the function kill seems very powerfull,we can make our processuses more resiliant if we block the kills
	for instance in c, if we use the signal() function.

	but we can always kill using the -9 parameter but it will kill the process very variantly and we will have not chances to recover anything

	So i wanna code something which will be an "immortal process" and will be unkillable.

	how? it's very simple to explain, inside of the kernel, there is a system call called kill(), we need to hook into this system call inside of the kernel, replace it with our own function kill().
	if it's our immortal process, then it will give an error message or sopmething, i will figure this out. otherwise, it will run the original process.
	-----------------------------------------------------

	---------------------------What's the plan? ----------------------
	In order to make this project, i will use these sources i found online:
		-The book "The linux kernel module programming guide by Peter.J Michael.B, Ori.P Bob.M and Jim.H which will help me write my first module which will be "Hello world" example
	which is open source.Here's the link: https://sysprog21.github.io/lkmpg/
		-In order to help me to hook in the syscall table, i found a git by BrunoCiccarino, a GPL-3.0 license open source. It is a simple guide on how to get to the system calls and change them.Here's the link: https://github.com/BrunoCiccarino/LinuxLowlevelAddict
	(atm, i dunno if this guide will work on my OS, but we'll see and if it doesnt i will either:
																								-change kernel
																								-change Technique (which i hope i won't have to)
	------------------------------------------------------------------------------------------------------------
	------------------------ !!!!!!!!!!!!!!!!!!!!!!!! BEWARE ! : I RECOMMAND YOU TO USE A VM IF U WANT TO MODIFY YOUR KERNEL, IT WILL SAVE YOU A LOT OF ISSUES !!!!!!!!!!!-------------------
The first step is to find the system call table (SCT) and it's what it sounds like, it's a table of the operating system calls.
It is defined inside of the linux kernel.

so for example, lets say we have an array of void pointers called sys_call_table:
	void *sys_call_table[1] = SYS_exit // exit()
	void *sys_call_table[4] = SYS_write // write()
	void *sys_call_table[1] etc...;
there are more than 200 system calls.

Back in kernel 2.4 or 2.6 it was easy to find them because they were exported as regular variables, so we could just   #include some .h files and then we could get the adresse of the SCT.
So, if we wanted to change the write call, we could just change the adress of the wreite function in the array (for eg, sys_call_table[SYS_write]) and give it the adress of my own function.

But we're not on thios version, so we need to find the SCT, plus there is write protection inside of the cpu
which will also need to avoid.

if we're lucky, we can find this directory: /proc/kallsyms and inside of it, there is the sys call table.
I don't think i'm that lucky, but let's give it a try.
It's me 30seconds into the future, i'm not lucky.
We can try something else, there is a system map thing on linux which has a saved version of the SCT.
It should be at /boot/System.map-$(uname -r)
I found this inside of the system map file: 
ffffffffffffffff B The real System.map is in the linux-image-<version>-dbg package
so elts try to isntall it

for the next steps, just like when we debug, we can set like a breakpoint somewhere in the code
and when the execution of the programm reaches thzat poiont, it pauses.
Its simillar, but we do it on the cpu, hardware level.
Then we have to get pass the CPU write protection because currently it's on READ ONLY.
There is a cpu register (cr0) and it contains different flags (010111000) with 0 meaning OFF
and 1 meaning ON.
we'll write a function similar to this:
			static inline void unprotection_memory(void) {
				write_cr0_forced(cr0 & ~0x00010000);
			
			}
we need to change one of these bit from one to zero and then we'll be able to alter the syscall table.
then we'll change it back afterwards.
It will be easy since we'll be on kernel mode.

So, i found the adress of the sys call table by installoing the debug version of my cureent kernel:
ffffffff82000320 D sys_call_table.
but it can be scrambled, sometimes, these are OFFSET by KASLR which randomize the adress
everytime we reboot.
after checking, the system call for printk inside my system.map is different from the adress inside the 
kallsyms, which means that KASLR is indeed enabled.
	void *sys_call_table[1]
seems weird, but the adress of the systable might be f8000000
in order to get the linux image, i used: 
    dpkg --list | grep linux | grep dbg
then:
    dpkg -L linux-image-6.1.0-29-amd64-dbg | grep -E 'System.*ap'

in order to get this: /usr/lib/debug/boot/System.map-6.1.0-29-amd64 
so this is the sysmap path
in order to find the adress easily, i made a bash script to automate the search
SO, the address of my syscalltable is: 0xffffffff8a000320
