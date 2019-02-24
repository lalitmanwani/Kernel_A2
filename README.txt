Lalit Manwani
CS-550
lmanwan1@binghamton.edu
B00764797


1. Copy the numpipe.c characted device program in the linux-4.1.1/kernel folder.
2. Copy and compile the producer.c and consumer.c at any location.
3. Copy the Makefile in the linux-4.1.1/kernel folder.
4. Compile the numpipe.c program by entering the following command in the same folder as the Makefile.
5. Insert the numpipe character device as a kernel module using the following command, specifying a size for the buffer in the place of N:
	
	sudo insmod numpipe.ko N=100

6. On running the producer.c program, the producer process writes data of 1 byte to the buffer maintained by the character device.
7. On running the consumer.c program, the consumer process consumes data of 1 byte from the buffer.
8. Concurrency has been implemented using semaphores.