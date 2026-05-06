FLAGS= -DDEBUG -x cu
LIBS= -lm -lcudart
CC=nvcc

ALWAYS_REBUILD=makefile

nbody: nbody.o compute.o
	$(CC) $(FLAGS) $^ -o $@ $(LIBS)

nbody.o: nbody.c planets.h config.h vector.h $(ALWAYS_REBUILD)
	$(CC) $(FLAGS) -c nbody.c -o nbody.o

compute.o: compute.c config.h vector.h $(ALWAYS_REBUILD)
	$(CC) $(FLAGS) -c compute.c -o compute.o

clean:
	rm -f *.o nbody
