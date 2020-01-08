CC = gcc
CFLAGS = -g -pedantic
LINK = 
TARGET = uni_editor
objects = editor.o term_driver.o strings.o utils.o

all: $(objects)
	$(CC) $(CFLAGS) $(LINK) $(objects) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o

