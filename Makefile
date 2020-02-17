build:
	gcc -o demo demo.c acl.c
	gcc -o ls ls.c acl.c
	gcc -o myfputs fput.c acl.c
	gcc -o myfgets fget.c acl.c
	gcc -o create_dir create_dir.c acl.c
run:
	gcc -o demo demo.c acl.c
	./demo
clean:
	rm demo
debug:
	gcc -g -o demo demo.c acl.c
	gcc -g -o ls ls.c acl.c
