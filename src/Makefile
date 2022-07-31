CFLAGS=-Wall -O3 -pedantic
OBJS=valley.o utils.o scenario.o movement.o finds.o monsters.o combat.o

all: default

default: valley

valley: $(OBJS)
	gcc $(CFLAGS) -o valley $(OBJS) -lncurses -lm

.cpp.o:
	gcc $(CFLAGS) -c -o $@ $<

clean:
	rm -f valley $(OBJS)

combat.o: combat.cpp utils.h header.h
finds.o: finds.cpp header.h utils.h
monsters.o: monsters.cpp header.h
movement.o: movement.cpp header.h utils.h
scenario.o: scenario.cpp header.h utils.h
utils.o: utils.cpp utils.h
valley.o: valley.cpp utils.h header.h
