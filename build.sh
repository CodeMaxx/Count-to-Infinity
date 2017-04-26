#!/usr/bin/env bash

g++ -std=c++11 -g -pthread -lsodium -lsqlite3 -lldap -Wno-deprecated-declarations server.cpp -o server
g++ -std=c++11 -g -pthread -lsqlite3 client.cpp -o client
