build: src/main_server.cpp src/client.cpp src/server.cpp src/server.h src/user.h src/balance.h
	g++ -O3 -std=c++11 src/server.cpp src/main_server.cpp -o server
	g++ -O3 -std=c++11 src/client.cpp -o client

clean:
	rm server client bin/*.o
