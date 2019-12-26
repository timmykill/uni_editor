CC = gcc
CFLAGS = -g -pedantic
LINK = 
TARGET = uni_editor 
objects = editor.o

all: $(objects)
	$(CC) $(CFLAGS) $(LINK) $(objects) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

install:
	mkdir -p $(DESTDIR)/usr/bin/
	cp ./sbopkg_c $(DESTDIR)/usr/bin/sbopkg_c

clean:
	rm -f *.o

