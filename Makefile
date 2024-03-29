# Enter 'make' to build files
# Enter 'make clean' to remove build files
# Run './shell' after build for program

TARGET = shell
LIBS = -lm
CC = gcc
CFLAGS = -g -Wall

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)