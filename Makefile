CC=g++
ringmaster: ringmaster.cpp check.h potato.h
	$(CC) -o ringmaster ringmaster.cpp check.h potato.h

