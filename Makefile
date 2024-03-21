BINARY=./build/fsh
CODEDIRS=./src ./src/server ./src/client
BUILDDIR=./build

CC=gcc
CFLAGS=-Wall -MP -MD

CFILES=$(foreach dir, $(CODEDIRS), $(wildcard $(dir)/*.c))
OBJECTS=$(patsubst ./src/%.c, ./build/%.o, $(CFILES))
DEPFILES=$(patsubst %.c, $(BUILDDIR)/%.d, $(CFILES))
BUILDDIRS = $(patsubst ./src%, ./build%, $(CODEDIRS))

all: builddirs $(BINARY)

$(BINARY): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ 

$(BUILDDIR)/%.o: ./src/%.c
	$(CC) $(CFLAGS) -c -o $@ $^

builddirs:
	@mkdir -p $(BUILDDIRS)

clean:
	rm -rf $(BUILDDIR)

run: all
	@$(BINARY)

list: all
	@$(BINARY) list

chunk: all
	@$(BINARY) chunk a665a45920422f9d417e4867efdc4fb8a04a1f3fff1fa07e998e86f7f7a27ae3
