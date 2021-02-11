CFLAGS = -std=c99 -Wall -Wextra -Wno-missing-field-initializers -g3 -Os

xbc : xbc.c

test:
	./xbc ws/a.xb
	./xbc ws/b.xb
	./xbc ws/c.xb
	./xbc ws/d.xb
	./xbc ws/e.xb
	./xbc ws/f.xb
	./test_xbc.sh

clean:
	rm -rf xbc
	rm -rf ws/a
	rm -rf ws/b
	rm -rf ws/c
	rm -rf ws/d
	rm -rf ws/e
	rm -rf ws/f
