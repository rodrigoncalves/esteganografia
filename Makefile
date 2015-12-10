all:
	g++ -o main main.cpp -std=c++11 -g -lcrypto

server:
	g++ -o server server.c -std=c++11 -g -lpthread

clean:
	rm -f main server
