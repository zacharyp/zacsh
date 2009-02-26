CFLAGS = -g -Wall
OBJECTS = zacsh.o eveccommand.o history.o

all: mysleep zacsh

zacsh : $(OBJECTS)
	cc $(CFLAGS) $(OBJECTS) -o zacsh

zacsh.o : zacsh.c
	cc $(CFLAGS) -c zacsh.c

eveccommand.o : eveccommand.c
	cc $(CFLAGS) -c eveccommand.c

history.o : history.c
	cc $(CFLAGS) -c history.c

mysleep : mysleep.c
	cc $(CFLAGS) -o mysleep mysleep.c

clean:
	rm -f core zacsh mysleep $(OBJECTS)
