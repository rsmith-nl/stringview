# file: Makefile
# vim:fileencoding=utf-8:ft=make
#
# Copyright © 2025 R.F. Smith <rsmith@xs4all.nl>
# SPDX-License-Identifier: MIT
# Created: 2025-04-07T22:53:50+0200
# Last modified: 2025-04-08T19:10:02+0200

# Define the C compiler to be used, if not the default cc.
#CC = gcc13

# The next line is for building optimized libraries.
CFLAGS = -Os -pipe -std=c11 -fPIC -ffast-math
LFLAGS += -s -pipe -fmerge-constants

# The next line is for building debugging libraries.
CFLAGS = -pipe -std=c11 -fPIC -g -Wall -Wextra -Wstrict-prototypes -Wpedantic \
		 -Wshadow-all -Wmissing-field-initializers -Wpointer-arith

# Extra libraries to be linked.
LIBS += 

# Where to search for additional header files.
#HDIRS = -I/usr/X11R6/include

# Where to install the library and header file.
# THESE SHOULD BE CHECKED AND CHANGED IF NECESSARY!
LDEST = /usr/local/lib
HDEST = /usr/local/include
MDEST = /usr/local/share/man/man3

# 'lib' wil be prepended automagically to the basename.
# for libfoo.so.0.1.2 the basename is foo, version_major is 0,
# version_minor is 1 and patchlevel is 2.
BASENAME      = str
VERSION_MAJOR = 0
VERSION_MINOR = 0
PATCHLEVEL    = 1

# Name of the library header file.
HDRS = str.h

# list of source files
SRC = str.c

# Name of the manual pages.
MPAGES = .3

# Regular files to include in the distribution
DISTFILES = Makefile README LICENSE $(MPAGES)

# Name and source of the test program
TEST = test
TESTSRC = $(TEST).c

# Extra stuff to add into the distribution.
XTRA_DIST = 

##### No editing necessary beyond this point ####
.SUFFIXES:
.SUFFIXES: .c .o .3 .txt .pdf .html

.PHONY:	clean help all shared static install_shared install_static \
install uninstall dist is_root check_dir log depend

NUM2      = $(VERSION_MAJOR).$(VERSION_MINOR)
NUM3      = $(VERSION_MAJOR).$(VERSION_MINOR).$(PATCHLEVEL)
SHARED	 = lib$(BASENAME).so.$(NUM3)
STATIC	 = lib$(BASENAME).a
PKGDIR    = lib$(BASENAME)-$(NUM3)
TARFILE	 = lib$(BASENAME)-$(NUM3).tar.gz
OBJS		 = $(SRC:.c=.o)
LOG 		 = ChangeLog

INSTALL   = install

# This is the default target.
help::
	@echo "Command  Meaning"
	@echo "-------  -------"
	@sed -n -e '/##/s/:.*\#\# /\t/p' -e '/@sed/d' Makefile

static:		$(STATIC)  ## build static library
shared:		$(SHARED)  ## build shared library
all:		$(SHARED) $(STATIC)  ## build static and shared library
test: $(TEST)  ## build a test program

$(SHARED):	$(OBJS)
	$(CC) -shared -Wl,-soname,lib$(BASENAME).so.$(VERSION_MAJOR) \
	-o $(SHARED) $(OBJS) $(LIBS)

$(STATIC): $(OBJS)
	ar crus $(STATIC) $(OBJS)

$(TEST): $(TESTSRC) $(STATIC)
	$(CC) -o $(TEST) $(TESTSRC) $(STATIC)

.PHONY: style
style:  ## Reformat source code using astyle.
	astyle -n *.c *.h

.PHONY: tidy
tidy:  ## Run static code checker clang-tidy.
	clang-tidy19 --quiet *.c *.h

clean:;  ## remove all generated files
	-rm -f $(OBJS) core *~ $(SHARED) $(STATIC) $(TARFILE) \
	$(LOG) $(TEST) $(MPAGES:.3=.ps) \
	$(MPAGES:.3=.txt) $(MPAGES:.3=.html)

# Check if the user has root privileges.
is_root:;
	@if [ `id -u` != 0 ]; then \
		echo "You must be root to install the library!"; \
		exit 1; \
	fi

# Check if the install directory isn't the current directory.
check_dir:;
	@if [ $(LDEST) = . -o $(HDEST) = . ]; then \
		echo "Can't install in current directory!";\
		exit 1; \
	fi

install_hdr: $(HDRS)  ## install the header files
	list='$(HDRS)'; \
	for i in $$list; do \
		rm -f $(MDEST)/$$i; \
		$(INSTALL) -m 644 $$i $(MDEST); \
	done

install_man: $(MPAGES)  ## install the manual pages
	list='$(MPAGES)'; \
	for i in $$list; do \
		rm -f $(MDEST)/$$i; \
		$(INSTALL) -m 644 $$i $(MDEST); \
	done

install_shared: $(SHARED) is_root check_dir install_hdr install_man  ## install the shared library
   # Remove the old library.
	rm -f $(LDEST)/$(SHARED)
	rm -f $(LDEST)/lib$(BASENAME).so.$(VERSION_MAJOR)
	rm -f $(LDEST)/lib$(BASENAME).so
   # Install new library and make the usual links.
	$(INSTALL) -m 755 $(SHARED) $(LDEST) ; cd $(LDEST) ;\
	ln -s $(SHARED) lib$(BASENAME).so.$(VERSION_MAJOR) ;\
	ln -s $(SHARED) lib$(BASENAME).so 

install_static: $(STATIC) is_root check_dir install_hdr install_man  ## install the static library
   # Remove the old library.
	rm -f $(LDEST)/$(STATIC)
   # Install new library.
	$(INSTALL) -m 644 $(STATIC) $(LDEST)

install: install_shared install_static ## installs both libraries

uninstall: is_root  ## remove the installed libraries, header and man-page
	rm -f $(LDEST)/$(SHARED)
	rm -f $(LDEST)/$(STATIC)
	rm -f $(LDEST)/lib$(BASENAME).so.$(VERSION_MAJOR)
	rm -f $(LDEST)/lib$(BASENAME).so
	list='$(HDRS)'; \
	for i in $$list; do \
		rm -f $(HDEST)/$$i; \
	done
	list='$(MPAGES)'; \
	for i in $$list; do \
		rm -f $(MDEST)/$$i; \
	done

dist: $(LOG)  ## make a source distribution file.
	rm -rf $(PKGDIR)
	mkdir -p $(PKGDIR)
	cp $(DISTFILES) $(XTRA_DIST) *.c *.h $(PKGDIR)
	tar -czf $(TARFILE) $(PKGDIR)
	rm -rf $(PKGDIR)

pdf: $(MPAGES:.3=.ps)  ## manual pages in PDF format

txt: $(MPAGES:.3=.txt)  ## manual pages in utf-8 text format

html: $(MPAGES:.3=.html) ## manual pages in html format

# Implicit rule (is needed because of HDIRS!)
.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) $(HDIRS) -c -o $@ $<

.3.pdf:
	mandoc -Tpdf -mdoc $< >$@

.3.txt:
	mandoc -Tlatin1 -mdoc $< >$@

.3.html:
	mandoc -Thtml -mdoc $< >$@

# Run 'make depend' to record the dependencies of sources on headers.
depend:
	$(CC) -MM $(CFLAGS) $(SRCS) >Makefile.depend

# Include the dependencies generated by 'make depend' after this line.
.if exists(Makefile.depend)
.include "Makefile.depend"
.endif
