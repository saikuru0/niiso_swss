CC = g++
CFLAGS = -std=c++11 -Wall -pthread -Iniiso/json/include
LDFLAGS = -lssl -lcrypto

SRC = main.cpp
LIBS = niiso/niiso.hpp simplewss/client_wss.hpp
TARGET = app

all: $(TARGET)

$(TARGET): $(SRC) $(LIBS)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LDFLAGS)

clean:
	rm $(TARGET)

.PHONY: all clean
