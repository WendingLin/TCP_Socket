CC=g++
all: ringmaster player

ringmaster: ringmaster.cpp check.h potato.h
	$(CC) -o ringmaster ringmaster.cpp check.h potato.h
player: player.cpp check.h
	$(CC) -o player player.cpp check.h 
