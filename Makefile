CC=gcc
CFLAGS=-Wall -Wextra -Wformat-nonliteral -Wcast-align -Wpointer-arith \
-Wbad-function-cast -Wmissing-prototypes -Wstrict-prototypes \
-Wmissing-declarations -Winline -Wundef -Wnested-externs -Wcast-qual -Wshadow \
-Wwrite-strings -Wno-unused-parameter -Wno-declaration-after-statement \
-Wfloat-equal -pedantic -ansi -Werror -std=c17
SRCDIR=src
LIBDIR=lib
TESTDIR=test
DOCDIR=doc
BIN=greenland_*

make: test client server
	doxygen Doxyfile
	astyle --project $(LIBDIR)/*.c $(LIBDIR)/*.h $(SRCDIR)/*.c $(TESTDIR)/*.c
	./tests

test: date.o species.o tree.o menuoption.o test.o
	$(CC) $(CFLAGS) -pthread -lcheck unit_tests.o menuoption.o tree.o tree_state.o species.o date.o -o tests

test.o: $(TESTDIR)/unit_tests.c
	$(CC) $(CFLAGS) -c $(TESTDIR)/*.c

client: tree.o menuoption.o client.o
	$(CC) $(CFLAGS) client.o menuoption.o tree.o tree_state.o date.o species.o -o greenland_client

client.o: $(SRCDIR)/client.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/client.c

server: tree.o menuoption.o server.o
	$(CC) $(CFLAGS) server.o tree.o tree_state.o menuoption.o date.o species.o -o greenland_server

server.o: $(SRCDIR)/server.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/server.c

tree.o: tree_state.o $(LIBDIR)/tree.h $(LIBDIR)/tree.c
	$(CC) $(CFLAGS) -c $(LIBDIR)/tree.h $(LIBDIR)/tree.c

tree_state.o: $(LIBDIR)/tree_state.h $(LIBDIR)/tree_state.c
	$(CC) $(CFLAGS) -c $(LIBDIR)/tree_state.h $(LIBDIR)/tree_state.c

species.o: $(LIBDIR)/species.h $(LIBDIR)/species.c
	$(CC) $(CFLAGS) -c $(LIBDIR)/species.h $(LIBDIR)/species.c

menuoption.o: date.o tree.o $(LIBDIR)/menuoption.h $(LIBDIR)/menuoption.c
	$(CC) $(CFLAGS) -c $(LIBDIR)/menuoption.h $(LIBDIR)/menuoption.c

date.o: $(LIBDIR)/date.h $(LIBDIR)/date.c
	$(CC) $(CFLAGS) -c $(LIBDIR)/date.h $(LIBDIR)/date.c

clean:
	rm *.o $(BIN)
