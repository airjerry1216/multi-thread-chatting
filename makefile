all:
	gcc -o server -pthread server.c
	gcc -o client -pthread client.c	
	
