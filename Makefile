CC = gcc
CFLAGS = -g -pedantic
LINK = 
TARGET = uni_editor
DOXYFILE = Doxyfile
objects = editor.o term_driver.o file.o  utils.o types.o second_buffer.o

.PHONY : docs

all: $(objects)
	$(CC) $(CFLAGS) $(LINK) $(objects) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

docs:
	doxygen $(DOXYFILE)

clean:
	rm -f *.o

