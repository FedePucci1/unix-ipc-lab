#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <dirent.h>

static sem_t sem;
static sem_t semCierre;

#define MAX_LENGTH 100
#define NUM_STRINGS 10
#define MAX_BUF 1024
#define MAX_BUF_RUTA 2048
#define LONGITUD_MAXIMA_CADENA 1000

static int leer_ruta(int descriptor, char *destino, size_t capacidad)
{
  ssize_t leidos = read(descriptor, destino, capacidad - 1);
  if (leidos < 0)
  {
    perror("No se pudo leer la ruta");
    return -1;
  }

  destino[leidos] = '\0';
  return 0;
}

int main(int argc, char *argv[])
{
  sem_t *semaphore = sem_open("sem999", O_CREAT, 0600, 0);
  sem_t *semaphore2 = sem_open("semCierre999", O_CREAT, 0600, 0);
  sem_t *semaphoreFile = sem_open("semFile999", O_CREAT, 0600, 0);
  sem_t *semaphoreFileAcces = sem_open("semFileAcces999", O_CREAT, 0600, 0);
  sem_t *semaphoreStart = sem_open("semaphoreStart999", O_CREAT, 0600, 0);
  int t, value, tr, activo = 1, i = 0, longitudDelArreglo;
  char tuberia[] = "./fifos/fifooo";
  // char tuberiaRegreso[] = "./fifos/fifoRegreso";
  char msj[MAX_BUF], msjRuta[MAX_BUF_RUTA];
  char vecPermisos[NUM_STRINGS][MAX_LENGTH] = {""};
  char vecConfigPath[NUM_STRINGS][MAX_LENGTH] = {""};
  char buffRuta[200], buffRutaConfig[200], aux[200];
  char auxRetorno[MAX_BUF_RUTA];
  char *auxiliar;
  struct dirent *dp, *dpConfig;
  DIR *dir, *dirConfig;

  if (argc != 2)
  {
    printf("\nFalta Parametro.\n");
    return EXIT_FAILURE;
  }

  FILE *archivo = fopen(argv[1], "r");     // Modo lectura
  char buffConfig[LONGITUD_MAXIMA_CADENA]; // Aquí vamos a ir almacenando cada línea
  while (fgets(buffConfig, LONGITUD_MAXIMA_CADENA, archivo))
  {
    strtok(buffConfig, "\n");
    strcpy(vecPermisos[i], buffConfig);
    // printf("\n %s", vecPermisos[i]);
    realpath(vecPermisos[i], buffRutaConfig);
    // strcpy(vecConfigPath[i], buffRutaConfig);
    // printf("\nValorBufferConfig: %s\n", buffRutaConfig);
    i++;
  }
  longitudDelArreglo = i;

  // return 0;

  // Activo permiso para inicio de cliente
  // sem_post(semaphoreStart);

  while (activo)
  {
    sem_getvalue(semaphore, &value);
    printf("\nThe value of the semaphore is %d\n", value);

    // decremento semaforo
    sem_wait(semaphore);

    //printf("**************\n");
    t = open(tuberia, O_RDONLY);
    //printf("\nvalor de apertura fifo: %d\n", t);
    //printf("**************\n");

    // lee el mensaje desde la tuberia y lo almacena
    read(t, msj, MAX_BUF);

    // imprime el valor que comparte el otro proceso
    printf("Recibido: %s\n", msj);


    if (strcmp(msj, "lstdir") == 0)
    {
      //printf("Directorios Operables:\n");
      //printf("-------------------------------\n");
      // strcpy(auxRetorno, vecPermisos[0]);
      // strcat(auxRetorno, "\n");
      strcpy(auxRetorno, "");
      for (int j = 0; j < longitudDelArreglo; j++)
      {
        //printf("%s\n", vecPermisos[j]);
        strcat(auxRetorno, vecPermisos[j]);
        strcat(auxRetorno, "\n");
      }
    }
    else if (strcmp(msj, "srvstp") == 0)
    {
      activo = 0;
    }
    else if (strcmp(msj, "mkfile") == 0 || strcmp(msj, "rdfile") == 0 || strcmp(msj, "delfile") == 0)
    {
      if (strcmp(msj, "mkfile") == 0)
      {
        sem_post(semaphoreFile);
        sem_wait(semaphoreFileAcces);
        if (leer_ruta(t, msjRuta, sizeof(msjRuta)) == -1)
          continue;
        printf("Ruta recibida: %s \n", msjRuta);
        /**parseo parametro**/
        auxiliar = strrchr(msjRuta, '/');
        *auxiliar = '\0';
        printf("Ruta recibida parseada: %s \n", msjRuta);
        auxiliar++;
        printf("Archivo a crear: %s \n", auxiliar);
        /********/
        dir = opendir(msjRuta);
        dp = readdir(dir);
        realpath(dp->d_name, buffRuta);
        printf("%s\n", buffRuta);
        system("touch ./archivosCreados/borrar.txt");
        sem_post(semaphoreFile);
      }
      else if (strcmp(msj, "rdfile") == 0)
      {
        sem_post(semaphoreFile);
        sem_wait(semaphoreFileAcces);
        if (leer_ruta(t, msjRuta, sizeof(msjRuta)) == -1)
          continue;
        printf("Ruta recibida: %s \n", msjRuta);
        /**parseo parametro**/
        auxiliar = strrchr(msjRuta, '/');
        *auxiliar = '\0';
        printf("Ruta recibida parseada: %s \n", msjRuta);
        auxiliar++;
        printf("Archivo a crear: %s \n", auxiliar);
        /********/
        dir = opendir(msjRuta);
        dp = readdir(dir);
        realpath(dp->d_name, buffRuta);
        printf("%s\n", buffRuta);
        system("touch ./archivosCreados/borrar.txt");
        sem_post(semaphoreFile);
      }
      else
      {
        sem_post(semaphoreFile);
        sem_wait(semaphoreFileAcces);
        if (leer_ruta(t, msjRuta, sizeof(msjRuta)) == -1)
          continue;
        printf("Ruta recibida: %s \n", msjRuta);
        /**parseo parametro**/
        auxiliar = strrchr(msjRuta, '/');
        *auxiliar = '\0';
        printf("Ruta recibida parseada: %s \n", msjRuta);
        auxiliar++;
        printf("Archivo a crear: %s \n", auxiliar);
        /********/
        dir = opendir(msjRuta);
        dp = readdir(dir);
        realpath(dp->d_name, buffRuta);
        printf("%s\n", buffRuta);
        system("touch ./archivosCreados/borrar.txt");
        sem_post(semaphoreFile);
      }
    }

    // abre la tuberia
    t = open(tuberia, O_WRONLY);
    printf("\nvalor de apertura fifo escritura: %d\n", t);

    // Grabar fifo de regreso
    // write(t, "Mensaje de Regreso", sizeof("Mensaje de Regreso"));
    write(t, auxRetorno, sizeof(auxRetorno));

    // incremento semaforo
    sem_post(semaphore2);
  }

  close(t);

  return 0;
}
