all: scu.c
	arm-linux-gnueabihf-gcc -O3 -o scu scu.c
install: scu
	scp scu root@bruford.cs.unc.edu:/tmp/
debug: scu.c
	arm-linux-gnueabihf-gcc -g -o scu scu.c
clean: scu.c
	rm scu

