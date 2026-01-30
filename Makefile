all:
	cc lab3d.c -o lab3d -lncurses -s

clean:
	rm lab3d

install:
	cp lab3d /usr/bin
	chmod a+rx /usr/bin/lab3d

uninstall:
	rm /usr/bin/lab3d
