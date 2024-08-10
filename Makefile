#---------------------------------------------------------------------
# Arquivo	: Makefile
# Conteúdo	: compilar o programa simulador
# Autor		: Cecília Junqueira V. M. Pereira (ceciliajunq@ufmg.br), baseado no Makefile do prof. Wagner Meira Jr. (meira@dcc.ufmg.br)
# Histórico	: 2023-10-03 - arquivo criado
#---------------------------------------------------------------------
# Opções	: make all - compila tudo e executa exemplo
#		: make clean - remove objetos e executável
#---------------------------------------------------------------------

CC = g++
LIBS = -lm
SRC = src
OBJ = obj
INC = include
BIN = bin
OBJS = $(OBJ)/simulador.o
CFLAGS = -g -pg -Wall -std=c++17 -c -I$(INC)

EXE = $(BIN)/simulador.out

all: use mem

mem: $(EXE)
	$(EXE) 4096 1024 1 ./testes/file.txt
	$(EXE) 4096 1024 2 ./testes/file.txt

use:
	mkdir -p bin/
	mkdir -p obj/
	
$(BIN)/simulador.out: $(OBJS)
	$(CC) -g -o $(BIN)/simulador.out $(OBJS) $(LIBS)

$(OBJ)/simulador.o: $(HDRS) $(SRC)/simulador.cpp
	$(CC) $(CFLAGS) -o $(OBJ)/simulador.o $(SRC)/simulador.cpp

clean:
	rm -f $(EXE) $(OBJS) gmon.out