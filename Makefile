# file: Makefile
# vim:fileencoding=utf-8:ft=make
#
# Copyright © 2025 R.F. Smith <rsmith@xs4all.nl>
# SPDX-License-Identifier: MIT
# Created: 2025-04-07T22:53:50+0200
# Last modified: 2025-08-31T12:44:16+0200

# Define the C compiler to be used, if not the default cc.
#CC = gcc13

# The next lines are for building optimized libraries.
#CFLAGS = -Os -pipe -std=c11 -fPIC -ffast-math
# Linker flags for optimized libraries.
#LFLAGS += -s -pipe -fmerge-constants 

# The next lines are for building debugging libraries
CFLAGS = -pipe -std=c11 -fPIC -g3 -Wall -Wextra -Wstrict-prototypes -Wpedantic \
	-Wshadow-all -Wmissing-field-initializers -Wpointer-arith \
	-fsanitize=address,undefined
# Linker flags for debugging
LFLAGS += -s -pipe -fmerge-constants -fsanitize=address,undefined

# Extra libraries to be linked.
LIBS += -lm

# Where to search for additional header files.
#HDIRS = -I/usr/X11R6/include

# Name of the library header file.
HDRS = stringview.h

# list of source files
SRC = stringview.c

# Name and source of the test program
TESTSRC = test.c

##### No editing necessary beyond this point ####
.SUFFIXES:
.SUFFIXES: .c .o .3 .txt .pdf .html

.PHONY:	clean help all shared static install_shared install_static \
install uninstall dist is_root check_dir log depend

# This is the default target.
help::
	@echo "Command  Meaning"
	@echo "-------  -------"
	@sed -n -e '/##/s/:.*\#\# /\t/p' -e '/@sed/d' Makefile

graphs: sv8tod.pdf sv8toi.pdf  ## build FSM graphs for sv8toi and sv8tod.

sv8tod.pdf: sv8tod.dot
	dot -Tpdf -osv8tod.pdf sv8tod.dot

sv8toi.pdf: sv8toi.dot
	dot -Tpdf -osv8toi.pdf sv8toi.dot

test: $(TESTSRC) $(SRC)  ## build test program
	$(CC) $(CFLAGS) -o test $(TESTSRC) $(SRC) $(LIBS)

.PHONY: style
style:  ## Reformat source code using astyle.
	astyle -n *.c *.h

.PHONY: tidy
tidy:  ## Run static code checker clang-tidy.
	clang-tidy19 --quiet *.c *.h --

clean:;  ## remove all generated files
	-rm -f $(OBJS) core *~ $(SHARED) $(STATIC) $(TARFILE) \
	$(LOG) test $(MPAGES:.3=.ps) \
	$(MPAGES:.3=.txt) $(MPAGES:.3=.html) \
	sv8tod.pdf sv8toi.pdf

# Implicit rule (is needed because of HDIRS!)
.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) $(HDIRS) -c -o $@ $<
