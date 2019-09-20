CC = arm-linux-gnueabihf-gcc
all: scu.c
	$(CC) -O3 -o scu $^
install: scu
	scp scu root@bruford.cs.unc.edu:/tmp/
debug: scu.c
	$(CC) -g -o scu $^
clean: scu.c
	rm scu

