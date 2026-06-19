#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>

static sem_t sem, semCierre;
#define MAX_BUF 1024

void ayuda()
{
	char *mensaje = "\n==============\nAyuda\n==============\n\nDescripcion Cliente:\nCompilacion: gcc cliente.c -o entrada -pthread \nEjecucion: ./entrada\n==============\n";
	printf("%s", mensaje);
	exit(1);
}

int main(int argc, char *argv[])
{
	int t, value, tr, valueFile;
	char tuberia[] = "./fifos/fifooo";
	// char tuberiaRegreso[] = "./fifos/fifoRegreso";
	char msj[MAX_BUF];
	sem_t *semaphore = sem_open("sem999", O_CREAT, 0600, 0);
	sem_t *semaphore2 = sem_open("semCierre999", O_CREAT, 0600, 0);
	sem_t *semaphoreFile = sem_open("semFile999", O_CREAT, 0600, 0);
	sem_t *semaphoreFileAcces = sem_open("semFileAcces999", O_CREAT, 0600, 0);
	sem_t *semaphoreStart = sem_open("semaphoreStart999", O_CREAT, 0600, 0);

	// Ayuda
	if (argc == 2)
	{
		if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
		{
			ayuda();
		}
	}

	if (argc == 3)
	{
		if (strcmp(argv[1], "mkfile") != 0 && strcmp(argv[1], "rdfile") != 0 && strcmp(argv[1], "delfile") != 0)
		{
			printf("\nParametros incorrectos.\n");
			return EXIT_FAILURE;
		}
	}
	else if (argc == 2)
	{
		if (strcmp(argv[1], "lstdir") != 0 && strcmp(argv[1], "srvstp") != 0)
		{
			printf("\nParametros incorrectos.\n");
			return EXIT_FAILURE;
		}
	}
	else
	{
		printf("\nCantidad de parametros incorrectos.\n");
		return EXIT_FAILURE;
	}

	// Semaforo de inicio de Cliente
	// sem_wait(semaphoreStart);

	printf("\nEscribo %s en el fifo.\n", argv[1]);
	/*crea un nuevo archivo fifo especial,
	incluye una ruta y un parametro con una
	mascara de permisos.
	*/
	mkfifo(tuberia, 0666);
	// mkfifo(tuberiaRegreso, 0666);

	// abre tuberia de escritura
	t = open(tuberia, O_CREAT | O_WRONLY);
	printf("\nvalor de apertura fifo: %d\n", t);

	// muestro valor semaforo
	sem_getvalue(semaphore, &value);
	printf("\nThe value of the semaphore is %d\n", value);

	// incremento semaforo
	sem_post(semaphore);

	// muestro valor semaforo
	sem_getvalue(semaphore, &value);
	printf("\nThe value of the semaphore is %d\n", value);

	// grabar en fifo
	// write(t, "Mensaje entre Procesos", sizeof("Mensaje entre Procesos"));
	if (argc == 3)
	{
		sem_getvalue(semaphoreFile, &valueFile);
		printf("\nThe value of the semaphoreFile is %d\n", valueFile);

		write(t, argv[1], sizeof(argv[1]));
		//printf("\nEstoyAca\n");
		//printf("\nValor argumento 2: %s", argv[2]);
		//printf("\nValor tamanio argumento 2: %d", strlen(argv[2]));
		sem_wait(semaphoreFile);
		write(t, argv[2], strlen(argv[2]));
		sem_post(semaphoreFileAcces);
		sem_wait(semaphoreFile);
	}
	else
	{
		write(t, argv[1], sizeof(argv[1]));
	}

	// decremento semaforo********************
	printf("\n===========ESPERANDO RESPUESTA===================\n");
	// muestro valor semaforo cerrado al inicio
	sem_getvalue(semaphore2, &value);
	printf("\nThe value of the semaphore2 is %d\n", value);
	sem_wait(semaphore2);
	//printf("\nse supone q grabamos\n");

	// abre la tuberia de lectura
	// tr = open(tuberiaRegreso, O_RDONLY);
	// printf("\n valor de apertura fifoRegreso: %d\n", tr);

	// abre tuberia de escritura
	t = open(tuberia, O_RDONLY);
	printf("\nvalor de apertura fifoRetorno Lectura: %d\n", t);

	// lee el mensaje desde la tuberia y lo almacena
	read(t, msj, MAX_BUF);
	printf("\nRecibido: %s \n", msj);

	// cerramos la tuberia
	close(t);
	// close(tr);

	// borramos
	unlink(tuberia);
	// unlink(tuberiaRegreso);

	return 0;
}