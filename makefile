CC = gcc
FILES = ./UDPclient.c
OUT_EXE = ping

build: $(FILES)
	$(CC) -o $(OUT_EXE) $(FILES)

run:
	./$(OUT_EXE)
