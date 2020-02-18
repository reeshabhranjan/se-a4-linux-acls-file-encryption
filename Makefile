build:
	gcc -g -o mydemo demo.c acl.c security.c
	gcc -g -o myls ls.c acl.c security.c
	gcc -g -o myfput fput.c acl.c security.c
	gcc -g -o myfget fget.c acl.c security.c
	gcc -g -o mycreate_dir create_dir.c acl.c security.c
	gcc -g -o mydo_exec do_exec.c acl.c security.c
	gcc -g -o getacl getacl.c acl.c security.c
	gcc -g -o setacl setacl.c acl.c security.c

perm:
	chown root:root myls
	chown root:root myfput
	chown root:root myfget
	chown root:root mycreate_dir
	chown root:root mydo_exec
	chown root:root mydemo
	chown root:root getacl
	chown root:root setacl
	chmod u+s myls
	chmod u+s myfput
	chmod u+s myfget
	chmod u+s mycreate_dir
	chmod u+s mydo_exec
	chmod u+s mydemo
	chmod u+s getacl
	chmod u+s setacl

clean:
	rm ./mydemo ./myls ./myfput ./myfget ./mycreate_dir ./mydo_exec ./getacl ./setacl
