CC=gcc
CFLAGS=-Wall -Wextra -Wformat-nonliteral -Wcast-align -Wpointer-arith \
-Wbad-function-cast -Wmissing-prototypes -Wstrict-prototypes \
-Wmissing-declarations -Winline -Wundef -Wnested-externs -Wcast-qual -Wshadow \
-Wwrite-strings -Wno-unused-parameter -Wfloat-equal -pedantic -ansi -Werror
SRCDIR=src
LIBDIR=lib
DOCDIR=doc
BIN=greenland

make: main
	doxygen Doxyfile
	astyle --project $(SRCDIR)/*.c

main: main.o
	$(CC) $(CFLAGS) main.o -o greenland 

main.o: $(SRCDIR)/main.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/main.c

clean:
	rm *.o $(BIN)
