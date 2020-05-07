EECS 3221 Project 3
Name: Giseung Kim
Student #: 212981858
EECS login: giseung

I have two version of the memory management unit
mmu.c is the first part of the project without page replacement
mmuLRU.c is the modified second part of the project with LRU page replacement
I did not modify test.sh as I did not use it for the means of testing. I did it manually myself on terminal

So the command for first mmu.c will be:
	gcc mmu.c
	./a.out BACKING_STORE.bin addresses.txt

And the command for second mmuLRU.c will be:
	gcc mmuLRU.c
	./a.out BACKING_STORE.bin addresses.txt