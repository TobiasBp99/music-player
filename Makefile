CC=gcc
CFLAGS=-c -Wall
LDFLAGS= -Wall

PATH_S = ./BavassoPiizzi_Tobias/src
PATH_B = ./BavassoPiizzi_Tobias/bin
PATH_H = ./BavassoPiizzi_Tobias/header

OBJ1 = $(PATH_S)/servidor.o $(PATH_S)/funciones.o
OBJ2 = $(PATH_S)/cliente.o $(PATH_S)/funciones.o

all:
	make servidor
	make cliente

servidor: $(OBJ1)
	$(CC) -o $@ $(OBJ1) $(LDFLAGS)
	rm $(PATH_S)/funciones.h
	mv $(OBJ1) $(PATH_B)

cliente: $(OBJ2)
	$(CC) -o $@ $(OBJ2) $(LDFLAGS)
	rm $(PATH_S)/funciones.h
	mv $(OBJ2) $(PATH_B)

clean:
	rm servidor
	rm cliente
	rm $(PATH_B)/funciones.o
	rm $(PATH_B)/servidor.o
	rm $(PATH_B)/cliente.o

new:
	make clean
	make all

runserver:
	make servidor
	./servidor

runclient:
	make cliente
	./cliente

%.o: %.c Makefile
	cp $(PATH_H)/funciones.h $(PATH_S) 
	$(CC) $(CFLAGS) $< -o $@
