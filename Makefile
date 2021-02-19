PROGNAME = pwgen

CC = gcc
RM = rm -f
MKDIR_P = mkdir -p
INSTALL_DATA = install -m 0644
INSTALL_PROGRAM = install -m 0755

CFLAGS = -Wall -Ofast
LFLAGS =

prefix = /usr
bindir = $(prefix)/bin
mandir = $(prefix)/share/man


.PHONY: all test install uninstall clean

all: $(PROGNAME) $(PROGNAME).1.gz

include $(wildcard *.d)

$(PROGNAME): pwgen.o
	$(CC) -MMD -MP $^ -o $@ $(LFLAGS)

%.o: %.c
	$(CC) -MMD -MP -c $< -o $@ $(CFLAGS)

%.gz: %
	cat $< | gzip -f >$@

test: $(PROGNAME)
	./$(PROGNAME) --lower --upper
	./$(PROGNAME) -n 32 -lud
	./$(PROGNAME) -n 32 -a
	./$(PROGNAME) -n 32 -c 01

install:
	$(MKDIR_P) $(bindir) $(mandir)/man1
	$(INSTALL_PROGRAM) $(PROGNAME) $(bindir)
	$(INSTALL_DATA) $(PROGNAME).1.gz $(mandir)/man1

uninstall:
	$(RM) $(bindir)/$(PROGNAME)
	$(RM) $(mandir)/man1/$(PROGNAME).1.gz

clean:
	$(RM) $(PROGNAME) *.o *.d *.gz
