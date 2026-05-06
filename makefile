FLAGS= -DDEBUG
LIBS= -lm -lcudart
NVCC=nvcc

nbody: nbody.o compute.o
	$(NVCC) $(FLAGS) $^ -o $@ $(LIBS)

nbody.o: nbody.c planets.h config.h vector.h compute.h
	$(NVCC) $(FLAGS) -c nbody.c -o nbody.o

compute.o: compute.c config.h vector.h
	$(NVCC) $(FLAGS) -c compute.c -o compute.o

clean:
	rm -f *.o nbody
