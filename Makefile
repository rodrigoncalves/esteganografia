all:
	g++ -o main main.cpp -std=c++11 -g -lcrypto

server:
	gcc -o server server.c -g -lpthread

clean:
	rm -f main server
