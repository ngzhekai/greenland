CC=gcc
CFLAGS=-Wall -Wextra -Wformat-nonliteral -Wcast-align -Wpointer-arith \
-Wbad-function-cast -Wmissing-prototypes -Wstrict-prototypes \
-Wmissing-declarations -Winline -Wundef -Wnested-externs -Wcast-qual -Wshadow \
-Wwrite-strings -Wno-unused-parameter -Wfloat-equal -pedantic -ansi -Werror -Wno-discarded-qualifiers
SRCDIR=src
LIBDIR=lib
TESTDIR=test
DOCDIR=doc
BIN=greenland

make: test main
	doxygen Doxyfile
	astyle --project $(LIBDIR)/*.c $(LIBDIR)/*.h $(SRCDIR)/*.c $(TESTDIR)/*.c
	./tests

test: species.o tree.o menuoption.o test.o
	$(CC) $(CFLAGS) -pthread -lcheck unit_tests.o menuoption.o tree.o tree_state.o species.o -o tests

test.o: $(TESTDIR)/unit_tests.c
	$(CC) $(CFLAGS) -c $(TESTDIR)/*.c

main: tree.o menuoption.o main.o
	$(CC) $(CFLAGS) main.o menuoption.o tree.o tree_state.o -o greenland 

main.o: $(SRCDIR)/main.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/main.c

tree.o: tree_state.o $(LIBDIR)/tree.h $(LIBDIR)/tree.c
	$(CC) $(CFLAGS) -c $(LIBDIR)/tree.h $(LIBDIR)/tree.c

tree_state.o: $(LIBDIR)/tree_state.h $(LIBDIR)/tree_state.c
	$(CC) $(CFLAGS) -c $(LIBDIR)/tree_state.h $(LIBDIR)/tree_state.c

species.o: $(LIBDIR)/species.h $(LIBDIR)/species.c
	$(CC) $(CFLAGS) -c $(LIBDIR)/species.h $(LIBDIR)/species.c

menuoption.o: $(LIBDIR)/menuoption.h $(LIBDIR)/menuoption.c
	$(CC) $(CFLAGS) -c $(LIBDIR)/menuoption.h $(LIBDIR)/menuoption.c

clean:
	rm *.o $(BIN)
