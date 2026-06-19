.PHONY: clean

all: Ejercicio2APL3

Ejercicio2APL3:
	g++ -o Ejercicio2APL3 Ejercicio2APL3.cpp -lpthread
	
Ejercicio2APL3.o: Ejercicio2APL3.cpp 
	g++ -c Ejercicio2APL3.cpp

clean:
	rm -f *.o
