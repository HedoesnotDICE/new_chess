CC = g++
cc = g++

target = Server

cflags = -std=c++14 -g -Wall

libs = -lpthread -llog4cpp -lmysqlcppconn -lprotobuf

defines = -DMY_DEBUG

objects = main.o tool.o net_msg.pb.o CServer.o CMysql.o

$(target) : $(objects)
	$(CC) -o $(target) $(objects) $(libs) $(cflags)

main.o : main.cpp 
	$(CC) -c main.cpp 

tool.o : tool.cpp tool.h
	$(CC) -c tool.cpp

net_msg.pb.o : net_msg.pb.cc net_msg.pb.h
	$(CC) -c net_msg.pb.cc 

CServer.o : CServer.cpp CServer.h
	$(CC) -c CServer.cpp $(defines) $(cflags)

CMysql.o : CMysql.cpp CMysql.h
	$(cc) -c CMysql.cpp 

.PHONY : clean
clean:
	rm $(target) $(objects)
