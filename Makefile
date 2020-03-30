build:
	gcc -g -o files/myls ls.c acl.c security.c
	gcc -g -o files/myfput fput.c acl.c security.c
	gcc -g -o files/myfget fget.c acl.c security.c
	gcc -g -o files/mycreate_dir create_dir.c acl.c security.c
	gcc -g -o files/mydo_exec do_exec.c acl.c security.c
	gcc -g -o files/getacl getacl.c acl.c security.c
	gcc -g -o files/setacl setacl.c acl.c security.c
	gcc -g -o files/testing testing.c acl.c security.c crypto.c

setup:
	apt install libssl-dev
	mkdir files

perm:
	chown root:root files/myls
	chown root:root files/myfput
	chown root:root files/myfget
	chown root:root files/mycreate_dir
	chown root:root files/mydo_exec
	chown root:root files/getacl
	chown root:root files/setacl
	chown root:root files/testing
	chmod u+s files/myls
	chmod u+s files/myfput
	chmod u+s files/myfget
	chmod u+s files/mycreate_dir
	chmod u+s files/mydo_exec
	chmod u+s files/getacl
	chmod u+s files/setacl
	chmod u+s files/testing

clean:
	rm files/myls files/myfput files/myfget files/mycreate_dir files/mydo_exec files/getacl files/setacl
