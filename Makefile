build:
	gcc -o demo demo.c acl.c
run:
	gcc -o demo demo.c acl.c
	./demo
clean:
	rm demo
debug:
	gcc -g -o demo demo.c acl.c
	gdb demo

