all:
	@make client
	@make server

client:
	g++ -o client main.cpp -std=c++11 -Wno-write-strings -lcrypto -g

server:
	gcc -o server server.c -g -lpthread

clean:
	rm -f main server
