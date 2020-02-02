CC=gcc
CFLAGS=-I.
OBJDIR=obj
LIBS=-lm

all: $(OBJDIR)/utils.o $(OBJDIR)/co-gpio.o $(OBJDIR)/main.o
	cc $(OBJDIR)/utils.o $(OBJDIR)/co-gpio.o $(OBJDIR)/main.o -o driver $(CFLAGS) $(LIBS)

lib: $(OBJDIR)/utils.o $(OBJDIR)/co-gpio.o 
	cc $(OBJDIR)/utils.o $(OBJDIR)/co-gpio.o -L. -llib/co-gpio

rebuild: clean all

$(OBJDIR)/utils.o: utils/utils.h utils/utils.c
	cc -o $@ -c utils/utils.c $(CFLAGS)

$(OBJDIR)/co-gpio.o: co-gpio/driver.h co-gpio/driver.c
	cc -o $@ -c co-gpio/driver.c $(CFLAGS)

$(OBJDIR)/main.o: main.c
	cc -o $@ -c main.c $(CFLAGS)

clean:
	rm -R driver obj/*
