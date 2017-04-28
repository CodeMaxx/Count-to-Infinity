#!/usr/bin/env bash

g++ -std=c++11 -g -Wno-deprecated-declarations server.cpp -o server -pthread -lsodium -lsqlite3 -lldap
g++ -std=c++11 -g -pthread -lsqlite3 client.cpp -o client
