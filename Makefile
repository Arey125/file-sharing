BINARY=./build/file-transfer
CODEDIRS=./src

CC=gcc
CFLAGS=-Wall -MP -MD

CFILES=$(foreach dir, $(CODEDIRS), $(wildcard $(dir)/*.c))
OBJECTS=$(patsubst ./src/%.c, ./build/%.o, $(CFILES))
DEPFILES=$(patsubst %.c, ./build/%.d, $(CFILES))

all: ./build $(BINARY)

$(BINARY): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ 

./build/%.o: ./src/%.c
	$(CC) $(CFLAGS) -c -o $@ $^

./build:
	mkdir -p ./build

clean:
	rm $(OBJECTS) $(DEPFILES) file-transfer

run: all
	$(BINARY)

files:
	@echo -n "/" | nc 127.0.0.1 9115

chunk:
	@echo -n "/chunk 1.txt" | nc 127.0.0.1 9115

