# -*- mode: makefile; -*-

OSNAME := $(shell uname)

WARNINGS = -W -Wall -Werror -Wno-deprecated-declarations

CFLAGS = -g -O -ansi -pedantic $(WARNINGS)

CPPFLAGS = -I.
LDFLAGS =

DEPENDDIR = ./.deps
DEPENDFLAGS = -M

SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c,%.o,$(SRCS))

TARGETS  = $(patsubst %.o,%,$(OBJS))

all: $(TARGETS)

C_DEPS = $(patsubst %.o,$(DEPENDDIR)/%.d,$(OBJS))
-include $(C_DEPS)

$(DEPENDDIR)/%.d: %.c $(DEPENDDIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(DEPENDFLAGS) $< >$@

$(DEPENDDIR):
	@[ ! -d $(DEPENDDIR) ] && mkdir -p $(DEPENDDIR)

%: %.c

%.o: %.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -o $@ -c $<

%: %.o
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	@rm -rf $(TARGETS) $(OBJS)

realclean:
	@rm -rf $(TARGETS) $(OBJS) $(DEPENDDIR)
