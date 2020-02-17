build:
	gcc -o mydemo demo.c acl.c
	gcc -o myls ls.c acl.c
	gcc -o myfput fput.c acl.c
	gcc -o myfget fget.c acl.c
	gcc -o mycreate_dir create_dir.c acl.c
	gcc -o mydo_exec do_exec.c acl.c

perm:
	chown root:root myls
	chown root:root myfput
	chown root:root myfget
	chown root:root mycreate_dir
	chown root:root mydo_exec
	chmod u+s myls
	chmod u+s myfput
	chmod u+s myfget
	chmod u+s mycreate_dir
	chmod u+s mydo_exec

clean:
	rm ./demo ./myls ./myfput ./myfget ./mycreate_dir ./mydo_exec
