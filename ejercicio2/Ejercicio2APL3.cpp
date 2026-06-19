//-----------------------Inicio Encabezado------------------------------//
// Nombre Ejercicio: "Ejercicio2APL3.cpp"
// Numero Trabajo Practico: 3
// Numero Ejercicio: 2
// Tipo: 1° Entrega
// Integrantes:
//
//       Gian Luca Simonetti
//       Federico Pucci
//       Agustin Contreras
//       Cristian Myroniuk
//       Simon Bentivenga
//-----------------------Fin del Encabezado-----------------------------//

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <thread>
#include <string>
#include <fstream>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <filesystem>
#include <vector>
#include <dirent.h>
#include <iomanip>
#include <list>
#include <semaphore.h>
#include <ctime>

using namespace std;
//#define NUM_THREADS 1

sem_t semTiempo;
sem_t semFin;
sem_t semIniciar;

void tiempo(int seg)
{
    sem_post(&semIniciar);
    sleep(seg);
    sem_wait(&semTiempo);
}

bool esPrimo(int numero)
{
    // Casos especiales
    if (numero == 0 || numero == 1 || numero == 4)
        return false;
    for (int x = 2; x < numero / 2; x++)
    {
        if (numero % x == 0)
            return false;
    }
    // Si no se pudo dividir por ninguno de los de arriba, sÃ­ es primo
    return true;
}

void contarPrimos(int tiempo, int h)
{
    int sem, num = 0, contados = 0;
    sem_wait(&semIniciar);
    sem_post(&semIniciar);
    sem_getvalue(&semTiempo, &sem);
    while (sem == 1)
    {
        num++;
        if (esPrimo(num))
            contados++;
        // cout << "ejecutando " << p << endl;
        sem_getvalue(&semTiempo, &sem);
    }
    cout << "el hilo " << h << " conto " << contados << " primos"<< endl;
    sem_post(&semFin);
}

int main(int argc, char *argv[])
{


    if (argc < 2 || argc == 2 && (strcmp(argv[1],"-h")==0 || strcmp(argv[1],"--help")==0))
    {
        printf("\nEsta ayuda se muestra llamando al proceso con un parámetro adicional -h o --help\n\n");
        printf("Este script realiza una cuenta de numeros primos\n");
        printf("valiendose de hilos informando cuantos numeros primos encuentran\n");
        printf("Ejemplo de ejecución: \n");
        printf("./Ejercicio2APL3 3 30\n");
        exit(0);
    }

    if (argc == 2 && (strcmp(argv[1],"-h")!=0 || strcmp(argv[1],"--help")!=0))
    {
        printf("Parámetro erróneo\n");
        exit(1);
    }

    ////////////////////////Declaraciones/////////////////////////
    pthread_t *threads;

    int argThread = atoi(argv[1]); // Cantidad de hilos a generar en el ejercicio
    int argTime = atoi(argv[2]);   // Tiempo que se asigna por parametro
    //////////////////////////////////////////////////////////////////

    if (argThread < 1)
    {
        cout << "Ingrese un parametro mayor o igual a 1" << endl;
        return 0;
    }

    sem_init(&semTiempo, 0, 1);
    sem_init(&semFin, 0, 0);
    sem_init(&semIniciar, 0, 0);
    //sleep(1);
    for (int i = 0; i < argThread; i++)
    {
        thread(contarPrimos, argTime, i).detach();
    }
    thread(tiempo, argTime).detach();
    for (int i = 0; i < argThread; i++)
    {
        sem_wait(&semFin);
    }
}
