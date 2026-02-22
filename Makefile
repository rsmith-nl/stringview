CFLAGS = -pipe -std=c11 -Wall -Wextra -Wstrict-prototypes -Wpedantic \
                -Wshadow -Wmissing-field-initializers -Wpointer-arith

check:  ## checks if the code builds cleanly. (default)
	$(CC) $(CFLAGS) -c stringview.c
	rm -f *.o

test: test.c stringview.c stringview.h  ## builds the test program.
	$(CC) $(CFLAGS) -o test test.c stringview.c -lm

graphs: sv8tod.pdf sv8toi.pdf  ## build FSM graphs for sv8toi and sv8tod. (requires graphviz)

sv8tod.pdf: sv8tod.dot
	dot -Tpdf -osv8tod.pdf sv8tod.dot

sv8toi.pdf: sv8toi.dot
	dot -Tpdf -osv8toi.pdf sv8toi.dot

.PHONY: clean
clean:  ## Remove generated files.
	rm -f *.o test sv8tod.pdf sv8toi.pdf

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
