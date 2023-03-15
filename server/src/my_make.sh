#!/bin/bash
g++ main.cpp tool.cpp -o Server -llog4cpp -lpthread -lmysqlcppconn -DMY_DEBUG
-lprotobuf
