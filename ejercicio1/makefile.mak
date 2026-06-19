.PHONY: clean

all: Ejercicio1APL3

Ejercicio1APL3:
	g++ -o Ejercicio1APL3 Ejercicio1APL3.cpp;
	
Ejercicio1APL3.o: Ejercicio1APL3.cpp
	g++ -c Ejercicio1APL3.cpp

clean:
	rm -f *.o
