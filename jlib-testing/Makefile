CC = /usr/bin/clang
CFLAGS = -pedantic -ansi -std=c99
DEBUGFLAGS = -fcolor-diagnostics -fansi-escape-codes \
			-O0 -Wall -Wextra -Wpedantic -g \
			-pedantic -ansi -std=c99 \
			-fsanitize=address -fsanitize-address-use-after-scope
LIBSRC = ../jlib-string/jlib-string.c
LIBFLAGS = -I ../jlib-string -I ../jlib-array


jlib-testing: jlib-testing.c $(LIBSRC)
	$(CC) $(CFLAGS) $(LIBFLAGS) -o $@ $^

debug: jlib-testing.c $(LIBSRC)
	$(CC) $(DEBUGFLAGS) $(LIBFLAGS) -o $@ $^

clean:
	rm -r debug.dSYM debug