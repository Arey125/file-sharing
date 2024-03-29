CODEDIRS=./src ./src/server ./src/client
BUILDDIR=./build
CFLAGS=-Wall -MP -MD
CC=gcc
DEBUG=0

ifeq ($(DEBUG), 1)
	BUILDDIR=./build/debug
	CFLAGS+= -fsanitize=address -fsanitize=undefined -fsanitize=leak -g -Og -static-libgcc
endif

BINARY=$(BUILDDIR)/fsh


CFILES=$(foreach dir, $(CODEDIRS), $(wildcard $(dir)/*.c))
OBJECTS=$(patsubst ./src/%.c, $(BUILDDIR)/%.o, $(CFILES))
DEPFILES=$(patsubst ./src/%.c, $(BUILDDIR)/%.d, $(CFILES))
BUILDDIRS = $(patsubst ./src%, $(BUILDDIR)%, $(CODEDIRS))

all: builddirs $(BINARY)

$(BINARY): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ 

$(BUILDDIR)/%.o: ./src/%.c
	$(CC) $(CFLAGS) -c -o $@ $^

builddirs:
	@mkdir -p $(BUILDDIRS)

clean:
	rm $(BINARY) $(OBJECTS) $(DEPFILES)

run: all
	@$(BINARY)

list: all
	@$(BINARY) list

chunk: all
	@$(BINARY) chunk a665a45920422f9d417e4867efdc4fb8a04a1f3fff1fa07e998e86f7f7a27ae3
