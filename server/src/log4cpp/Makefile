cc = g++
flags = -g
libs =  -lpthread -llog4cpp

log : main.o
	$(cc) -o log main.o

main.o : main.cpp
	$(cc) -c main.cpp -o main.o $(flags) $(libs)