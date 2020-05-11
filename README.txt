Design and Implementation of a Virtual Memory Unit (MMU)

This project is a program that translates logical to physical addresses for a virtual address space of size 216 = 65,536 bytes. This program read from a file containing logical addresses and, using a TLB and a page table, translate each logical address to its corresponding physical address and output the value of the byte stored at the translated physical address. This  simulates the steps involved in translating logical to physical addresses including resolving page faults using demand paging, managing a TLB, and implementing a page-replacement algorithm.

So the command for first mmu.c will be:
	gcc mmu.c
	./a.out BACKING_STORE.bin addresses.txt

And the command for second mmuLRU.c will be:
	gcc mmuLRU.c
	./a.out BACKING_STORE.bin addresses.txt
