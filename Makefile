CFLAGS = -std=c99 -Wall -Wextra -Wno-missing-field-initializers -g3 -Os

xbc : xbc.c

test:
	./xbc ws/a.xb
	./xbc ws/b.xb
	./test_xbc.sh

clean:
	rm -rf xbc
	rm -rf ws/a
	rm -rf ws/b
