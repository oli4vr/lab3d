all:
	cc lab3d.c -o lab3d -lncursesw -fexec-charset=UTF-8 -fwide-exec-charset=UTF-32 -O3

clean:
	rm lab3d

install:
	cp lab3d /usr/bin
	chmod a+rx /usr/bin/lab3d

uninstall:
	rm /usr/bin/lab3d
