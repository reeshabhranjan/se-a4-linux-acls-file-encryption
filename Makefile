build:
	gcc -o demo demo.c acl.c
	gcc -o ls ls.c acl.c
	gcc -o fput fput.c acl.c
	gcc -o fget fget.c acl.c
	gcc -o create_dir create_dir.c acl.c
	gcc -o do_exec do_exec.c acl.c
	sudo chmod u+s do_exec
	sudo chown root:root do_exec
demo:
	gcc -o demo demo.c acl.c
	./demo
clean:
	rm demo
debug:
	gcc -g -o demo demo.c acl.c
	gcc -g -o ls ls.c acl.c
