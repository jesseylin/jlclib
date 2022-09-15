CC = /usr/bin/clang
CFLAGS = -pedantic -ansi -std=c99
DEBUGFLAGS = -fcolor-diagnostics -fansi-escape-codes \
			-O0 -Wall -Wextra -Wpedantic -g \
			-pedantic -ansi -std=c99 \
			-fsanitize=address -fsanitize-address-use-after-scope
LIBSRC = 
LIBFLAGS = -I ../jlib-string -I ../jlib-array


jlib-string: jlib-string.c $(LIBSRC)
	$(CC) $(CFLAGS) $(LIBFLAGS) -o $@ $^

debug: jlib-string.c $(LIBSRC)
	$(CC) $(DEBUGFLAGS) $(LIBFLAGS) -o $@ $^

clean:
	rm -r debug.dSYM debug