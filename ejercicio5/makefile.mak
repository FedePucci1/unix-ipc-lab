all:
	g++ -o Cliente cliente.cpp -lrt -lpthread
	g++ -o Servidor servidor.cpp -lrt -lpthread
Cliente: 
	g++ -o Cliente cliente.cpp -lrt -lpthread
Servidor:
	g++ -o Servidor servidor.cpp -lrt -lpthread