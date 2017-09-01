CC:=g++
FLAG:=-std=c++11 -g -Ofast -funroll-loops -march=native

word2vec : main.o word2vec.o settings.h
	$(CC) -o word2vec main.o word2vec.o expTable.o $(FLAG) -lpthread -lm
	rm *.o

main.o : main.cpp word2vec.hpp vector.hpp settings.h counter.hpp 
	$(CC) -c main.cpp $(FLAG)

word2vec.o : word2vec.cpp vector.hpp settings.h counter.hpp  expTable.o
	$(CC) -c word2vec.cpp $(FLAG)

expTable.o : expTable.cpp expTable.hpp
	$(CC) -c expTable.cpp $(FLAG)

clean :
	rm word2vec
	rm *.o
