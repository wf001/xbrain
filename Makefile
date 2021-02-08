CFLAGS = -std=c99 -Wall -Wextra -Wno-missing-field-initializers -g3 -Os

xbc : xbc.c

% : %.xb
	@./$(word 2,$^) -o $@  $<

test:
	@./xbc ws/a.xb
	@./ws/a

remove:
	@rm -rf xbc
	@rm -rf ws/a
