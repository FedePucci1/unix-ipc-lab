//-----------------------Inicio Encabezado------------------------------//
// Nombre Ejercicio: "Ejercicio1APL3.cpp"
// Numero Trabajo Practico: 3
// Numero Ejercicio: 1
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
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

using namespace std;

int main(int argc, char const *argv[])
{
    if (argc == 2 && (strcmp(argv[1],"-h")==0 || strcmp(argv[1],"--help")==0))
    {
        printf("\nEsta ayuda se muestra llamando al proceso con un parámetro adicional -h o --help\n\n");
        printf("Este script implementa un árbol de procesos mediante varias llamadas a fork\n");
        printf("de forma tal que muestra los PID de los procesos con sus respectivos padres\n");
        printf("cumpliendo con un grafo dado por enunciado\n\n");
        printf("Ejemplo de ejecución: \n");
        printf("\tSoy el pid 1 (pid) padres: -\n");
        printf("\tSoy el pid 2 (pid) padres: 1 (pid1)\n");
        printf("\tSoy el pid 3 (pid) padres: 1 (pid1)\n");
        printf("\tSoy el pid 5 (pid) padres: 2 (pid2), 1 (pid1)\n");
        printf("\tSoy el pid 6 (pid) padres: 2 (pid2), 1 (pid1)\n");
        printf("\tSoy el pid 7 (pid) padres: 2 (pid2), 1 (pid1)\n");
        printf("\tSoy el pid 8 (pid) padres: -\n");
        printf("\tSoy el pid 9 (pid) padres: 5 (pid5), 2 (pid2), 1 (pid1)\n");
        printf("\tSoy el pid 10 (pid) padres: 7 (pid7), 2 (pid2), 1 (pid1)\n");
        printf("\tSoy el pid 11 (pid) padres: 8 (pid7)\n");
        printf("\tSoy el pid 12 (pid) padres: 5 (pid5), 2 (pid2), 1 (pid1)\n");
        exit(0);
    }

    if (argc == 2 && (strcmp(argv[1],"-h")!=0 || strcmp(argv[1],"--help")!=0))
    {
        printf("Parámetro erróneo\n");
        exit(1);
    }
    
    pid_t pid1 = fork();
    if (pid1 == 0)
    { // SOY EL P1
        cout << "PROCESO 1 - PID: " << getpid() << " - PADRE: " << getppid() << endl;
        pid_t pid2 = fork();
        if (pid2 == 0)
        {
            // SOY EL P2
            cout << "PROCESO 2 - PID: " << getpid() << " - PADRE: " << getppid() << endl;
            pid_t pid5 = fork();
            if (pid5 == 0)
            {
                // SOY EL P5
                cout << "PROCESO 5 - PID: " << getpid() << " - PADRE: " << getppid() << endl;
                pid_t pid12 = fork();
                if (pid12 == 0)
                {
                    // SOY EL P12
                    cout << "PROCESO 12 - PID: " << getpid() << " - PADRE: " << getppid() << endl;
                    return EXIT_SUCCESS;
                }
                else
                {
                    // SOY EL P5
                    wait(NULL);
                }
                return EXIT_SUCCESS;
            }
            else
            {
                // SOY EL P2
                pid_t pid6 = fork();
                if (pid6 == 0)
                {
                    // SOY EL P6
                    cout << "PROCESO 6 - PID: " << getpid() << " - PADRE: " << getppid() << endl;
                    pid_t pid9 = fork();
                    if (pid9 == 0)
                    {
                        // SOY EL P9
                        cout << "PROCESO 9 - PID: " << getpid() << " - PADRE: " << getppid() << endl;
                        return EXIT_SUCCESS;
                    }
                    else
                    {
                        // SOY EL P6
                        pid_t pid10 = fork();
                        if (pid10 == 0)
                        {
                            // SOY EL P10
                            cout << "PROCESO 10 - PID: " << getpid() << " - PADRE: " << getppid() << endl;
                            return EXIT_SUCCESS;
                        }
                        else
                        {
                            // SOY EL P6
                            wait(NULL);
                            wait(NULL);
                        }
                    }
                    return EXIT_SUCCESS;
                }
                else
                {
                    // SOY EL P2
                    wait(NULL);
                    wait(NULL);
                }
            }
            return EXIT_SUCCESS;
        }
        else
        {
            // SOY EL P1
            pid_t pid3 = fork();
            if (pid3 == 0)
            {
                cout << "PROCESO 3 - PID: " << getpid() << " - PADRE: " << getppid() << endl;
                pid_t pid7 = fork();
                if (pid7 == 0)
                {
                    // SOY EL P7
                    cout << "PROCESO 7 - PID: " << getpid() << " - PADRE: " << getppid() << endl;
                    return EXIT_SUCCESS;
                }
                else
                {
                    // SOY EL P3
                    wait(NULL);
                }
                return EXIT_SUCCESS;
            }
            else
            {
                // SOY EL P1
                wait(NULL);
                wait(NULL);
            }
        }
        return EXIT_SUCCESS;
    }
    else
    {
        wait(NULL);
        pid_t pid8 = fork();
        if (pid8 == 0)
        { // hijo 8 (p1)
            cout << "PROCESO 8 - PID: " << getpid() << " - PADRE: " << getppid() << endl;
            pid_t pid11 = fork();
            if (pid11 == 0)
            {
                // SOY EL P11
                cout << "PROCESO 11 - PID: " << getpid() << " - PADRE: " << getppid() << endl;
                return EXIT_SUCCESS;
            }
            else
            {
                wait(NULL);
            }
            return EXIT_SUCCESS;
        }
        else
        {
            wait(NULL);
        }
    }

    return EXIT_SUCCESS;
}
