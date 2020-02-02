CC=gcc
CFLAGS=-I.
OBJDIR=obj
LIBS=-lm

all: static shared

static: $(OBJDIR)/static/utils.o $(OBJDIR)/static/co-gpio.o
	mkdir -p lib/static
	ar rcs lib/static/co-gpio.a $(OBJDIR)/static/utils.o $(OBJDIR)/static/co-gpio.o

shared:
	mkdir -p lib/shared
	mkdir -p obj/shared
	cc -o obj/shared/utils.o -fPIC -c utils/utils.c
	cc -o obj/shared/driver.o -fPIC -c co-gpio/driver.c
	cc -shared -o lib/shared/co-gpio.so obj/shared/utils.o obj/shared/driver.o

rebuild: clean lib

$(OBJDIR)/static/utils.o: utils/utils.h utils/utils.c
	mkdir -p $(OBJDIR)/static
	cc -o $@ -c utils/utils.c $(CFLAGS)

$(OBJDIR)/static/co-gpio.o: co-gpio/driver.h co-gpio/driver.c
	mkdir -p $(OBJDIR)/static
	cc -o $@ -c co-gpio/driver.c $(CFLAGS)

clean:
	rm -f -R obj lib
