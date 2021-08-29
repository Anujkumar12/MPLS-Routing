all: main.cpp ./headers/network.h ./headers/routing.h ./headers/utils.h
	g++ -std=c++11 -I ./headers main.cpp -o routing
	
