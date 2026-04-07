# vim:fileencoding=utf-8:ft=make
# Use as many jobs as the computer has cores.
.MAKEFLAGS: -j C

CFLAGS = -pipe -std=c11 -Wall -Wextra -Wstrict-prototypes -Wpedantic \
                -Wshadow -Wmissing-field-initializers -Wpointer-arith

all: svtest single_header/stringview.h

svtest: svtest.c stringview.c stringview.h  ## builds the test program (default).
	$(CC) $(CFLAGS) -o svtest svtest.c stringview.c -lm

single_header/stringview.h: stringview.c stringview.h  ## Build single header library (POSIX only).
	cp stringview.h single_header/stringview.h
	echo "" >>single_header/stringview.h
	echo "#ifdef STRINGVIEW_IMPLEMENTATION" >>single_header/stringview.h
	tail -n +11 stringview.c >>single_header/stringview.h
	echo "" >>single_header/stringview.h
	echo "#endif // STRINGVIEW_IMPLEMENTATION" >>single_header/stringview.h

graphs: sv8tod.pdf sv8toi.pdf  ## build FSM graphs for sv8toi and sv8tod. (requires graphviz)

sv8tod.pdf: sv8tod.dot
	dot -Tpdf -osv8tod.pdf sv8tod.dot

sv8toi.pdf: sv8toi.dot
	dot -Tpdf -osv8toi.pdf sv8toi.dot

.PHONY: clean
clean:  ## Remove generated files.
	rm -f *.o svtest sv8tod.pdf sv8toi.pdf

.PHONY: style
style:  ## Reformat source code using astyle.
	astyle -n *.c *.h

.PHONY: tidy
tidy:  ## Check the code with clang-tidy
	clang-tidy19 --quiet *.c *.h 2>/dev/null

.PHONY: help
help:  ## List available commands
	@echo "make targets:"
	@sed -n -e '/##/s/:.*\#\#/\t/p' Makefile
	@echo
