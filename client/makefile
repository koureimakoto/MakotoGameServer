#################### CLIENT MAKEFILE ####################

# File installation path
prefix=/home/kourei/Documentos/MADFILE/FACULDADE/PROJETO_IFMS/PROJETO_LINUXGAMES/CODIGOS/GTCG/tcc_game/
# Binary path
bindir=$(prefix)/client_bin
# Compiler
CC=gcc
# Compiler Flags
CFLAGS=-g -Wall
CPTHDDFLAG=-lpthread
# Exec file
EXEC=mygame
# Source 
SRC= $(wildcard *.c)
OBJ= $(SRC:.c=.o)

NETSRC=network_lib/$(wildcard *.c)
NETOBJ=network_lib/$(NETSRC:.c=.o)

ERRORSRC=client_error_lib/$(wildcard *.c)
ERROROBJ=client_error_lib/$(ERRORSRC:.c=.o)

all:
    cd scenes; make 
    cd network_lib; make
    cd client_error_lib; make
    $(EXEC)

@echo "Compiling project"


mygame: $(ERROROBJ) $(NETOBJ) $(OBJ)
$(CC) -o $@ $^ %.o %.c
$(CC) $(CFLAGS) $(CPTHRFLAG) -o $@ -c $< 
main.o: main.c mygame.h
$(CC) $(CFLAGS) $(CPTHRFLAG) -o $@ -c $< 


install:all
@mv $(EXEC) $(bindir)/
.PHONY: clean mrproper

# Exec make clean *.o
clean:
    cd scenes; make clean
    cd network_lib; make clean
    cd client_error_lib; make clean




