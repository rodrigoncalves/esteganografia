all:
	mkdir -p bin/
	@make client
	@make server

client:
	g++ -o bin/client src/main.cpp -std=c++11 -Wno-write-strings -lcrypto -g

server:
	gcc -o bin/server src/server.c -g -lpthread

clean:
	rm -rf bin/ *.txt *.y
