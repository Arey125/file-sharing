CODEDIRS=./src ./src/server ./src/client ./src/sha256 ./src/sqlite ./src/db
BUILDDIR=./build
INCLUDES=-I./src
CFLAGS=-Wall -MP -MD $(INCLUDES)
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
	@$(BINARY) server

list: all
	@$(BINARY) list

chunk: all
	@$(BINARY) chunk ba9ca616847e94c76eb0ccf3407d192e2fcc2be925ecdf662bd2706ba3efd26d

wrongchunk: all
	@$(BINARY) chunk fa9ca616847e94c76eb0ccf3407d192e2fcc2be925ecdf662bd2706ba3efd26d

add: all
	@$(BINARY) add ./compile_commands.json
