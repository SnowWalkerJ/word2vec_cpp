CC:=g++
FLAG:=-std=c++11

main : main.o word2vec.o
	$(CC) -o main main.o word2vec.o $(FLAG)

main.o : main.cpp word2vec.hpp vector.hpp settings.h counter.hpp 
	$(CC) -c main.cpp $(FLAG)

word2vec.o : word2vec.cpp vector.hpp settings.h counter.hpp
	$(CC) -c word2vec.cpp $(FLAG)
