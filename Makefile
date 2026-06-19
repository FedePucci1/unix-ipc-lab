CXX := g++
CC := gcc
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic
CFLAGS := -std=gnu11 -Wall -Wextra -Wpedantic
THREAD_LIBS := -pthread
RT_LIBS := -lrt

.PHONY: all clean exercise1 exercise2 exercise3 exercise4 exercise5

all: exercise1 exercise2 exercise3 exercise4 exercise5

exercise1: ejercicio1/Ejercicio1APL3
exercise2: ejercicio2/primos
exercise3: ejercicio3/entrada ejercicio3/salida
exercise4: ejercicio4/cliente ejercicio4/servidor
exercise5: ejercicio5/cliente ejercicio5/servidor

ejercicio1/Ejercicio1APL3: ejercicio1/Ejercicio1APL3.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

ejercicio2/primos: ejercicio2/Ejercicio2APL3.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ $(THREAD_LIBS)

ejercicio3/entrada: ejercicio3/cliente.c
	$(CC) $(CFLAGS) $< -o $@ $(THREAD_LIBS) $(RT_LIBS)

ejercicio3/salida: ejercicio3/servidor.c
	$(CC) $(CFLAGS) $< -o $@ $(THREAD_LIBS) $(RT_LIBS)

ejercicio4/cliente: ejercicio4/cliente.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ $(THREAD_LIBS) $(RT_LIBS)

ejercicio4/servidor: ejercicio4/servidor.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ $(THREAD_LIBS) $(RT_LIBS)

ejercicio5/cliente: ejercicio5/cliente.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ $(THREAD_LIBS)

ejercicio5/servidor: ejercicio5/server.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ $(THREAD_LIBS)

clean:
	$(RM) ejercicio1/Ejercicio1APL3 ejercicio2/primos
	$(RM) ejercicio3/entrada ejercicio3/salida ejercicio3/fifos/*
	$(RM) ejercicio4/cliente ejercicio4/servidor
	$(RM) ejercicio5/cliente ejercicio5/servidor
