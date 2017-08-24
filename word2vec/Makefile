CC:=g++



main : main.o word2vec.o
	g++ -o main main.o word2vec.o -std=c++11

main.o : main.cpp word2vec.hpp vector.hpp settings.h counter.hpp 
	g++ -c main.cpp -std=c++11

word2vec.o : word2vec.cpp vector.hpp settings.h counter.hpp
	g++ -c word2vec.cpp -std=c++11
