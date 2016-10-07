CC=g++
CFLAGS=-c -Wall -std=c++11 \
		-I./http-parser/ \
		-I./libuv/include/ \
		-I./openssl/include/
LDFLAGS=-lpthread \
		./libuv/.libs/libuv.a \
		./openssl/libcrypto.a \
		./http-parser/http_parser.o
SOURCES=example.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=example

all: $(SOURCES) $(EXECUTABLE) run

$(EXECUTABLE): $(OBJECTS)
		$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
		$(CC) $(CFLAGS) $< -o $@

clean:
		rm -rf *.o $(EXECUTABLE)

run:
		DEBUG=* ./example http://www.google.com