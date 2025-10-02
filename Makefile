CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = programaTrab
OBJS = main.o funcionalidades.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) -lm

main.o: main.c estruturas.h
	$(CC) $(CFLAGS) -c main.c

funcionalidades.o: funcionalidades.c estruturas.h
	$(CC) $(CFLAGS) -c funcionalidades.c

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
