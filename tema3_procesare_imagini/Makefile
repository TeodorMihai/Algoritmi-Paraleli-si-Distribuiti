
CFLAGS = -Wall
COMPILER = mpicc
RUN = mpirun

build: tema.c
	$(COMPILER) $(CFLAGS) tema.c -o filtru

run: filtru
	$(RUN) -np 12 ./filtru topologie.in imagini.in statistica1.out

clean: 
	rm filtru
