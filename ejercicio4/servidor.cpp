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
#include <pthread.h>
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

#define TAM 1024
#define SMOBJ_NAME "/myMemoryObj"
#define SMOBJ_NAME2 "/myMemoryObj2"
#define SIZEOF_SMOBJ 200

using namespace std;

void help()
{
    cout << "Bienvenido al juego del ahoracado" << endl;
    cout << "Ingrese -h para ayuda" << endl;
    cout << "El juego permite hasta 3 jugadores y jugarse por medio de memoria compartida" << endl;
    cout << "Para ello no necesita especificar parametros " << endl;
    cout << "Ejemplo de ejecucion " << endl;
    cout << "./server" << endl;
}

int jugador[3];
int puntaje[3] = {0, 0, 0};
list<int> jugadores;
int cantJugadores = 3;
int i;
int turno = 0;
mutex mutexJugador[3];
string palabra = "casa";
string faltan = "casa";
string encotrada = "";
int cantLetras = palabra.length();
string letraUsadas = "";
thread th[3];
int vidas = 8;
sem_t semInicio;
sem_t semLeer;
sem_t semFin;
sem_t *leerDelCliente;
sem_t *escribirAlCliente;
bool termino = false;
string ultLetraIng;
string resMsj;
string resultadoPartida;

string mensaje(string condicion)
{
    string msj = "\n\tjugador: " + to_string(turno + 1);
    msj = msj + "\n\n\tletra ingresada: " + ultLetraIng;
    msj = msj + "\n\n\tcondicion: " + condicion;
    msj = msj + "\n\n\tpalabra: ";
    for (int i = 0; i < palabra.length(); i++)
    {
        msj = msj + encotrada[i] + " ";
    }
    msj = msj + "\n\n\tletras anteriores: ";
    for (int i = 0; i < letraUsadas.length(); i++)
    {
        msj = msj + letraUsadas[i] + " ";
    }
    msj = msj + "\n\n\tintentos restante: " + to_string(vidas);
    return msj;
}

string final()
{
    string msj = "";
    msj = msj + "\n\tpartida terminada";
    msj = msj + "\n\n\tresultado de la partida: " + resultadoPartida;
    msj = msj + "\n\n\tresultados de los competidores";
    for (int i = 0; i < cantJugadores; i++)
    {
        msj = msj + "\n\n\tjugador " + to_string(i + 1) + ": " + to_string(puntaje[i]) + " puntos";
    }
    return msj;
}

int siguiente(int j)
{
    if (j + 1 == cantJugadores)
        return 0;
    else
        return j + 1;
}

bool contiene(char l)
{
    for (int i = 0; i < letraUsadas.length(); i++)
    {
        if (l == letraUsadas[i])
            return true;
    }
    return false;
}

int ahorcado(char l, int j)
{
    bool acierto = false;
    string aux1 = "";
    string aux2 = "";
    cout << "evaluar " << l << endl;
    if (contiene(l))
        return 4;
    for (int i = 0; i < faltan.length(); i++)
    {
        if (faltan[i] == l)
        {
            aux1 = aux1 + "_";
            aux2 = aux2 + l;
            cantLetras--;
            acierto = true;
            puntaje[j] = puntaje[j] + 2;
        }
        else
        {
            aux1 = aux1 + faltan[i];
            aux2 = aux2 + encotrada[i];
        }
    }
    encotrada = aux2;
    faltan = aux1;
    if (acierto)
    {
        if (cantLetras == 0)
        {
            resultadoPartida = "GANADA";
            return 0; // gano
        }
        return 1; // acerto
    }
    vidas--;
    puntaje[j] = puntaje[j] - 1;
    if (vidas == 0)
    {
        resultadoPartida = "PERDIDA";
        return 2; // perdio
    }
    else
        return 3; // error
}

void jugar(int j, int fd)
{
    char *ptr;
    struct stat shmobj_st;
    char *leer;
    char escribir[1024] = "conectado al servidor\nesperando a los demas jugadores";
    char op;
    bool cortar = true;
    // send(socket, escribir, 1024, 0);
    ptr = (char *)mmap(NULL, sizeof(escribir), PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
    {
        printf("Map failed in write process: %s\n", strerror(errno));
        exit(1);
    }
    memcpy(ptr, escribir, sizeof(escribir));
    sem_post(escribirAlCliente);
    sleep(1);
    sem_wait(&semInicio);
    strcpy(escribir, "incio el juego, espera tu turno");
    // send(socket, escribir, 1024, 0);
    ptr = (char *)mmap(NULL, sizeof(escribir), PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
    {
        printf("Map failed in write process: %s\n", strerror(errno));
        exit(1);
    }
    memcpy(ptr, escribir, sizeof(escribir));
    sem_post(escribirAlCliente);
    sleep(1);
    while (cortar)
    {
        if (turno == j)
        {
            if (termino)
            {
                // strcpy(escribir, "TE");
                // strcat(escribir,letraUsadas.c_str());
                // send(socket, final().c_str(), 1024, 0);
                ptr = (char *)mmap(NULL, sizeof(escribir), PROT_WRITE, MAP_SHARED, fd, 0);
                if (ptr == MAP_FAILED)
                {
                    printf("Map failed in write process: %s\n", strerror(errno));
                    exit(1);
                }
                memcpy(ptr, escribir, sizeof(escribir));
                sem_post(escribirAlCliente);

                cortar = false;
                system("clear");
                cout << final() << endl;
            }
            else
            {
                // 0 - termino
                // 1 - jugar
                // 2 - esperar
                strcpy(escribir, "JU");
                strcat(escribir, letraUsadas.c_str());
                // send(socket, escribir, 1024, 0);
                ptr = (char *)mmap(NULL, sizeof(escribir), PROT_WRITE, MAP_SHARED, fd, 0);
                if (ptr == MAP_FAILED)
                {
                    printf("Map failed in write process: %s\n", strerror(errno));
                    exit(1);
                }
                memcpy(ptr, escribir, sizeof(escribir));
                sem_post(escribirAlCliente);

                // read(socket, leer, 2);
                sem_wait(leerDelCliente);

                ptr = (char *)mmap(NULL, shmobj_st.st_size, PROT_READ, MAP_SHARED, fd, 0);
                if (ptr == MAP_FAILED)
                {
                    printf("Map failed in read process: %s\n", strerror(errno));
                    printf("hola 1\n");
                    exit(1);
                }
                strcpy(leer, ptr);
                ultLetraIng = leer[0];
                // cout << "el jugador ingreso: " << leer[0] << endl;
                int res = ahorcado(ultLetraIng[0], j);
                strcpy(escribir, encotrada.c_str());
                switch (res)
                {
                case 1:
                    // strcat(escribir, "\nacertaste una letra");
                    resMsj = mensaje("acertada");
                    // send(socket, resMsj.c_str(), 1024, 0);
                    ptr = (char *)mmap(NULL, sizeof(resMsj.c_str()), PROT_WRITE, MAP_SHARED, fd, 0);
                    if (ptr == MAP_FAILED)
                    {
                        printf("Map failed in write process: %s\n", strerror(errno));
                        exit(1);
                    }
                    memcpy(ptr, resMsj.c_str(), sizeof(resMsj.c_str()));
                    sem_post(escribirAlCliente);
                    break;
                case 2:
                    // strcat(escribir, "\nperdiste");
                    resMsj = mensaje("errada");
                    // send(socket, resMsj.c_str(), 1024, 0);
                    ptr = (char *)mmap(NULL, sizeof(resMsj.c_str()), PROT_WRITE, MAP_SHARED, fd, 0);
                    if (ptr == MAP_FAILED)
                    {
                        printf("Map failed in write process: %s\n", strerror(errno));
                        exit(1);
                    }
                    memcpy(ptr, resMsj.c_str(), sizeof(resMsj.c_str()));
                    sem_post(escribirAlCliente);

                    termino = true;
                    break;
                case 0:
                    // strcat(escribir, "\nganaste");
                    resMsj = mensaje("acertada");
                    // send(socket, resMsj.c_str(), 1024, 0);
                    ptr = (char *)mmap(NULL, sizeof(resMsj.c_str()), PROT_WRITE, MAP_SHARED, fd, 0);
                    if (ptr == MAP_FAILED)
                    {
                        printf("Map failed in write process: %s\n", strerror(errno));
                        exit(1);
                    }
                    memcpy(ptr, resMsj.c_str(), sizeof(resMsj.c_str()));
                    sem_post(escribirAlCliente);

                    termino = true;
                    break;
                case 3:
                    // strcat(escribir, "\nla letra ingresada no es valida");
                    resMsj = mensaje("errada");
                    // send(socket, resMsj.c_str(), 1024, 0);
                    ptr = (char *)mmap(NULL, sizeof(resMsj.c_str()), PROT_WRITE, MAP_SHARED, fd, 0);
                    if (ptr == MAP_FAILED)
                    {
                        printf("Map failed in write process: %s\n", strerror(errno));
                        exit(1);
                    }
                    memcpy(ptr, resMsj.c_str(), sizeof(resMsj.c_str()));
                    sem_post(escribirAlCliente);

                    break;
                }
                turno = siguiente(j);
                // send(socket, mensaje().c_str(), 1024, 0);
                system("clear");
                cout << resMsj << endl;
                for (int i = 0; i < cantJugadores - 1; i++)
                {
                    sem_post(&semLeer);
                }
                sleep(1);
                letraUsadas = letraUsadas + ultLetraIng;
                // sem_post(&semInicio);
            }
        }
        else
        {
            if (!termino)
            {
                strcpy(escribir, "ES");
                strcat(escribir, letraUsadas.c_str());
                // send(socket, escribir, 1024, 0);
                ptr = (char *)mmap(NULL, sizeof(escribir), PROT_WRITE, MAP_SHARED, fd, 0);
                if (ptr == MAP_FAILED)
                {
                    printf("Map failed in write process: %s\n", strerror(errno));
                    exit(1);
                }
                memcpy(ptr, escribir, sizeof(escribir));
                sem_post(escribirAlCliente);

                sem_wait(&semLeer);
                // strcpy(escribir, "el otro jugador ingreso la letra ");
                // strcat(escribir, ultLetraIng.c_str());
                // send(socket, escribir, 1024, 0);
                // send(socket, resMsj.c_str(), 1024, 0);
                ptr = (char *)mmap(NULL, sizeof(resMsj.c_str()), PROT_WRITE, MAP_SHARED, fd, 0);
                if (ptr == MAP_FAILED)
                {
                    printf("Map failed in write process: %s\n", strerror(errno));
                    exit(1);
                }
                memcpy(ptr, resMsj.c_str(), sizeof(resMsj.c_str()));
                sem_post(escribirAlCliente);

                sleep(1);
            }
            else
            {
                // strcpy(escribir, "TE");
                // strcat(escribir,letraUsadas.c_str());
                cout << final() << endl;
                cortar = false;
                // send(socket, final().c_str(), 1024, 0);
                ptr = (char *)mmap(NULL, sizeof(final().c_str()), PROT_WRITE, MAP_SHARED, fd, 0);
                if (ptr == MAP_FAILED)
                {
                    printf("Map failed in write process: %s\n", strerror(errno));
                    exit(1);
                }
                memcpy(ptr, final().c_str(), sizeof(final().c_str()));
                sem_post(escribirAlCliente);

                system("clear");
                cout << final() << endl;
            }
        }
    }
    sem_post(&semFin);
    // close(socket);
    close(fd);
}

int main(int argc, char const *argv[])
{
    if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0))
    {
        help();
        exit(0);
    }

    if (argc == 2 && (strcmp(argv[1], "-h") != 0 || strcmp(argv[1], "--help") != 0))
    {
        printf("Parámetro erróneo\n");
        exit(1);
    }
    sem_init(&semInicio, 0, 0);
    sem_init(&semLeer, 0, 0);
    sem_init(&semFin, 0, 0);
    sem_t *semEsperandoJugadores = sem_open("semEsperandoJugadores01", O_CREAT, 0600, 0);
    leerDelCliente = sem_open("leerDelCliente01", O_CREAT, 0600, 0);
    escribirAlCliente = sem_open("escribirAlCliente01", O_CREAT, 0600, 0);
    int fd;
    char *ptr;
    struct stat shmobj_st;

    fd = shm_open(SMOBJ_NAME2, O_CREAT | O_RDWR, 0700); /* open s.m object*/
    if (fd == -1)
    {
        printf("Error file descriptor %s\n", strerror(errno));
        exit(1);
    }

    if (fstat(fd, &shmobj_st) == -1)
    {
        printf(" error fstat \n");
        exit(1);
    }

    system("clear");

    int size = 1024; // bytes
    int rc = ftruncate(fd, size);
    if (rc == -1)
    {
        cout << "Falló resize de memoria" << endl;
        exit(1);
    }

    for (i = 0; i < cantJugadores; i++)
    {
        int jugador;
        char *ptr;
        mutexJugador[i].lock();

        // jugador = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        cout << "esperando jugadores" << endl;
        sem_wait(semEsperandoJugadores);

        ptr = (char *)mmap(NULL, shmobj_st.st_size, PROT_READ, MAP_SHARED, fd, 0);

        if (ptr == MAP_FAILED)
        {
            printf("Map failed in read process: %s\n", strerror(errno));
            printf("hola 2\n");
            exit(1);
        }
        jugador = strtol(ptr, NULL, 10);

        if (jugador < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        cout << "se conecto el jugador " << i + 1 << endl;
        thread(jugar, jugador, fd).detach();
    }
    for (int i = 0; i < palabra.length(); i++)
    {
        encotrada = encotrada + "_";
    }
    for (i = 0; i < cantJugadores; i++)
    {
        sem_post(&semInicio);
    }
    for (i = 0; i < cantJugadores; i++)
    {
        sem_wait(&semFin);
    }
    return 0;
}
