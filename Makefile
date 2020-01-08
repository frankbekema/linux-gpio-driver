CC=gcc
CFLAGS=-I.
OBJDIR=obj
LIBS=-lm

edit: $(OBJDIR)/utils.o $(OBJDIR)/co-gpio.o $(OBJDIR)/main.o
	 $(CC) $(OBJDIR)/utils.o $(OBJDIR)/co-gpio.o $(OBJDIR)/main.o -o driver $(CFLAGS) $(LIBS)

$(OBJDIR)/utils.o: utils/utils.h utils/utils.c
	$(CC) -c -o $@ utils/utils.c $(CFLAGS)

$(OBJDIR)/co-gpio.o: co-gpio/driver.h co-gpio/driver.c
	$(CC) -c -o $@ co-gpio/driver.c $(CFLAGS)

$(OBJDIR)/main.o: main.c
	$(CC) -c -o $@ $< $(CFLAGS)