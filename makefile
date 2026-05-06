CC=nvcc

CFLAGS= -DDEBUG
LIBS= -lm -lcudart

ALWAYS_REBUILD=makefile

nbody: nbody.o compute.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

nbody.o: nbody.c planets.h config.h vector.h
	$(CC) $(CFLAGS) -c nbody.c -o nbody.o

compute.o: compute.c config.h vector.h
	$(CC) $(CFLAGS) -c compute.c -o compute.o

clean:
	rm -f *.o nbody
