CC = gcc
CFLAGS = -Wall
LDFLAGS =
SOURCES = main.c lib/ssd.c lib/com.c dht11/dht.c
OBJECTS  = $(SOURCES:.c=.o)
TARGET = main

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) -lwiringPi
clean:
	rm -f $(OBJECTS) $(TARGET)