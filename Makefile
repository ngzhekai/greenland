CC=gcc
CFLAGS=-Wall -Wextra -Wformat-nonliteral -Wcast-align -Wpointer-arith \
-Wbad-function-cast -Wmissing-prototypes -Wstrict-prototypes \
-Wmissing-declarations -Winline -Wundef -Wnested-externs -Wcast-qual -Wshadow \
-Wwrite-strings -Wno-unused-parameter -Wfloat-equal -pedantic -ansi -Werror
SRCDIR=src
LIBDIR=lib
TESTDIR=test
DOCDIR=doc
BIN=greenland

make: test main
	doxygen Doxyfile
	astyle --project $(LIBDIR)/*.c $(LIBDIR)/*.h $(SRCDIR)/*.c $(TESTDIR)/*.c
	./tests

test: tree.o test.o
	$(CC) $(CFLAGS) -pthread -lcheck unit_tests.o tree.o tree_state.o -o tests

test.o: $(TESTDIR)/unit_tests.c
	$(CC) $(CFLAGS) -c $(TESTDIR)/*.c

main: tree.o main.o
	$(CC) $(CFLAGS) main.o tree.o tree_state.o -o greenland 

main.o: $(SRCDIR)/main.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/main.c

tree.o: tree_state.o $(LIBDIR)/tree.h $(LIBDIR)/tree.c
	$(CC) $(CFLAGS) -c $(LIBDIR)/tree.h $(LIBDIR)/tree.c

tree_state.o: $(LIBDIR)/tree_state.h $(LIBDIR)/tree_state.c
	$(CC) $(CFLAGS) -c $(LIBDIR)/tree_state.h $(LIBDIR)/tree_state.c

clean:
	rm *.o $(BIN)
