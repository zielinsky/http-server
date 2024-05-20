CC = g++
CFLAGS = -Wall -Wextra -std=gnu++20 -O0
LDFLAGS =
SOURCES = main.cpp utils.cpp server.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = http-server

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS)

distclean: clean
	rm -f $(EXECUTABLE)

.PHONY: all clean distclean