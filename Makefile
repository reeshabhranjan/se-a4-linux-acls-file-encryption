build:
	gcc -o demo demo.c acl.c
	gcc -o ls ls.c acl.c
run:
	gcc -o demo demo.c acl.c
	./demo
clean:
	rm demo
debug:
	gcc -g -o demo demo.c acl.c
	gcc -g -o ls ls.c acl.c
	