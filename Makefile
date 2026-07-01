CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
TARGET = campeonato
SRCS = main.c aplicacao.c lista.c time.c partida.c bd_times.c bd_partidas.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)
