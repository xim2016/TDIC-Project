OBJS = GeneGlobDriver.o GenePanCanGlobDriver.o GTMatrix.o PanCanGTMatrix.o TDIMatrix.o TDIC_Main.o 
DEPS = GeneGlobDriver.h GenePanCanGlobDriver.h GTMatrix.h PanCanGTMatrix.h TDIMatrix.h

%.o: %.cpp $(DEPS)
	g++  -O3 -fopenmp -c $< -o $@

TDIC_GD_exeOMP: $(OBJS)
	g++  -O3 -fopenmp $(OBJS) -o $@

clean:
	rm -f *.o TDIC_GD_exeOMP


	
