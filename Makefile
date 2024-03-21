BINARY=./build/fsh
CODEDIRS=./src ./src/server ./src/client
BUILDDIR=./build

CC=gcc
CFLAGS=-Wall -MP -MD

CFILES=$(foreach dir, $(CODEDIRS), $(wildcard $(dir)/*.c))
OBJECTS=$(patsubst %.c, %.o, $(CFILES))
DEPFILES=$(patsubst %.c, $(BUILDDIR)/%.d, $(CFILES))

all: ./build $(BINARY)

$(BINARY): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ 

$(BUILDDIR)/%.o: %.c
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
	@echo -n "/chunk a665a45920422f9d417e4867efdc4fb8a04a1f3fff1fa07e998e86f7f7a27ae3" | nc 127.0.0.1 9115
