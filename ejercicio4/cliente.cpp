//-----------------------Inicio Encabezado------------------------------//
// Nombre Ejercicio: "Ejercicio4APL3.cpp"
// Numero Trabajo Practico: 3
// Numero Ejercicio: 4
// Tipo: 1° Entrega
// Integrantes:
//
//       Gian Luca Simonetti
//       Federico Pucci
//       Agustin Contreras
//       Cristian Myroniuk
//       Simon Bentivenga
//-----------------------Fin del Encabezado-----------------------------//

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <thread>
#include <fcntl.h>
#include <semaphore.h>
#include <string>
#include <mutex>
#include <list>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#define SMOBJ_NAME  "/myMemoryObj"
#define SIZEOF_SMOBJ 200

using namespace std;

void help()
{
    cout << "Bienvenido al juego del ahoracado" << endl;
    cout << "Ingrese -h para ayuda" << endl;
    cout << "Para empezar a jugar no necesita especificar parametros " << endl;
    cout << "Ejemplo de ejecucion " << endl;
    cout << "./cliente" << endl;
}

int main(int argc, char const *argv[])
{
    if (argc == 2 && (strcmp(argv[1],"-h")==0 || strcmp(argv[1],"--help")==0))
    {
        help();
        exit(0);
    }

    if (argc == 2 && (strcmp(argv[1],"-h")!=0 || strcmp(argv[1],"--help")!=0))
    {
        printf("Parámetro erróneo\n");
        exit(1);
    }
    char letra;
    int op;
    string escribir;
    char buffer[1024] = {0};
    bool termino = false;
    int fd;
    char *ptr;
    char *leer;
    struct stat shmobj_st;
    sem_t *semConectarJugador = sem_open("semEsperandoJugadores01", O_CREAT, 0600, 0);
    sem_t *escribirEnServer = sem_open("leerDelCliente01", O_CREAT, 0600, 0);
    sem_t *leerDelServer = sem_open("escribirAlCliente01", O_CREAT, 0600, 0);

    fd = shm_open (SMOBJ_NAME, O_CREAT | O_RDWR  , 0700); /* open s.m object*/
    
    if(fd == -1)
    {
        printf("Error file descriptor %s\n", strerror(errno));
        exit(1);
    }
    
    if(fstat(fd, &shmobj_st) == -1)
    {
        printf(" error fstat \n");
        exit(1);
    }

    system("clear");
    
    int size = 1024; // bytes
    int rc = ftruncate(fd, size);
    if(rc==-1){ cout << "Falló resize de memoria"<<endl; exit(1); }

    // if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    // {
    //     printf("\nConexion Fallida\n");
    //     return -1;
    // }

    int jugar=1;
    
    ptr = (char*)mmap(NULL, sizeof(int), PROT_WRITE, MAP_SHARED, fd, 0);
    if(ptr == MAP_FAILED)
    {
        printf("Map failed in write process: %s\n", strerror(errno));
        exit(1);
    }
    memcpy(ptr,&jugar, sizeof(int));
    sem_post(semConectarJugador);
    //read(sock, leer, 1024); // cliente conectado
    sem_wait(leerDelServer);
    leer = (char*)mmap(NULL, shmobj_st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if(leer == MAP_FAILED)
    {
        printf("Map failed in read process: %s\n", strerror(errno));
        printf("hola 1");
        exit(1);
    }
    cout << leer << endl;
    //read(sock, leer, 1024);
    sem_wait(leerDelServer);
    leer = (char*)mmap(NULL, shmobj_st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if(leer == MAP_FAILED)
    {
        printf("Map failed in read process: %s\n", strerror(errno));
        printf("hola 2");
        exit(1);
    }
    cout << leer << endl;
    system("clear");
    while (!termino)
    {
        //read(sock, leer, 1024);
        sem_wait(leerDelServer);
        leer = (char*)mmap(NULL, shmobj_st.st_size, PROT_READ, MAP_SHARED, fd, 0);
        if(leer == MAP_FAILED)
        {
            printf("Map failed in read process: %s\n", strerror(errno));
            printf("hola 3");
            exit(1);
        }
        string sub="";
        sub=leer[0];
        sub=sub+leer[1];
        string letra;
        bool repetida=false;
        if (strcmp(sub.c_str(), "JU") == 0)
        {
            cout << "es tu turno de jugar" << endl;
            do{
                if(repetida)
                    cout << "la letra ingresada ya fue usada" << endl;
                repetida=false;
                cout << "ingresa una letra: ";
                cin >> letra;
                for(int i = 2 ; i < strlen(leer);i++){
                    if(leer[i]==letra[0])
                        repetida=true;
                }
            }while(repetida);
            //send(sock, letra.c_str(), letra.length(), 0);
            ptr = (char*)mmap(NULL, sizeof(letra.c_str()), PROT_WRITE, MAP_SHARED, fd, 0);
            if(ptr == MAP_FAILED)
            {
                printf("Map failed in write process: %s\n", strerror(errno));
                exit(1);
            }
            memcpy(ptr,letra.c_str(), sizeof(letra.c_str()));
            sem_post(escribirEnServer);
            //read(sock, leer, 1024);
            sem_wait(leerDelServer);
            leer = (char*)mmap(NULL, shmobj_st.st_size, PROT_READ, MAP_SHARED, fd, 0);
            if(leer == MAP_FAILED)
            {
                printf("Map failed in read process: %s\n", strerror(errno));
                printf("hola 4");
                exit(1);
            }
            system("clear");
            cout << leer << "\n" << endl;
        }
        else if (strcmp(sub.c_str(), "ES") == 0)
        {
            cout << "espera tu turno para jugar" << endl;
            //read(sock, leer, 1024);
            sem_wait(leerDelServer);
            leer = (char*)mmap(NULL, shmobj_st.st_size, PROT_READ, MAP_SHARED, fd, 0);
            if(leer == MAP_FAILED)
            {
                printf("Map failed in read process: %s\n", strerror(errno));
                printf("hola 5");
                exit(1);
            }
            system("clear");
            cout << leer << "\n" << endl;
        }
        else
        {
            system("clear");
            cout << leer << endl;
            termino = true;
        }
    }
    return 0;
}
