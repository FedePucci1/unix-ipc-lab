.PHONY: clean

all: cliente servidor

cliente:
	g++ -o cliente cliente.cpp -pthread -lrt;
	
cliente.o: cliente.cpp 
	g++ -c cliente.cpp -pthread -lrt;

servidor:
	g++ -o servidor servidor.cpp -pthread -lrt;
	
servidor.o: servidor.cpp 
	g++ -c servidor.cpp -pthread -lrt;

clean:
	rm -f *.o
