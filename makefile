compiler=gcc
flags=-g -c -Wall

all: snapshot

snapshot: snapshot.o
	$(compiler) snapshot.o -o snapshot

snapshot.o: snapshot.c
	$(compiler) $(flags) snapshot.c

clean:
	rm -rf *.o snapshot

