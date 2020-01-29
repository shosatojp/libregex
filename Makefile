PROGRAM = libregex
CC = gcc
COPTION = -std=gnu99 -O3 -g
BINDIR = bin
SRCDIR = src
TARGET = $(BINDIR)/$(PROGRAM)
SRCS = $(shell find $(SRCDIR) -name *.c)


default: $(TARGET)

$(TARGET): $(SRCS)
	@mkdir -p $(BINDIR)
	$(CC) $^ -o $@ $(COPTION)

clean:
	rm -f $(TARGET)
	rm -r $(BINDIR)

run: $(TARGET)
	./$(TARGET)