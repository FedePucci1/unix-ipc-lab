#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <thread>
#include <fstream>
#include <fcntl.h>
#include <semaphore.h>
#include <string>
#include <mutex>
#include <vector>
#include <list>
#define PORT 60000
#define TAM 1024
#define TEXTO "AHORCADO";

using namespace std;

int jugador[3];
int puntaje[3] = {0, 0, 0};
list<int> jugadores;
int cantJugadores = 1;
int i;
int turno = 0;
mutex mutexJugador[3];
string palabra;
string faltan;
string encotrada;
int cantLetras;
string letraUsadas = "";
thread th[3];
int vidas = 8;
sem_t semInicio;
sem_t semLeer;
sem_t semFin;
bool termino = false;
string ultLetraIng;
string resMsj;
string resultadoPartida;

string palabraRandom(string path)
{
	int lineas = 0, i=0;
	string pal;
	vector<string> v;
	ifstream reader(path);
	if (reader.is_open())
	{
		while (std::getline(reader, pal))
			v.push_back(pal);
        srand(time(NULL));
		int random = rand() % v.size();
		pal = v.at(random);
		reader.close();
	}
	
	while(i < pal.length())
  	{
  		if(islower(pal[i]))
  		{
  			pal[i] = toupper(pal[i]);
		}
		i++;
  	}
	return pal;
}

void help()
{
    cout << "Bienvenido al juego del ahoracado" << endl;
    cout << "Ingrese -h para ayuda" << endl;
    cout << "Para empezar a jugar necesita especificar los siguientes parametros: " << endl;
    cout << "-p <numero de puerto> | necesario para la ejecucion" << endl;
    cout << "-ip <numero de ip> | necesario para la ejecucion" << endl;
    cout << "La forma correcta de especificarlos es: " << endl;
    cout << "./cliente -ip <num_ip> -p <num_puerto>"<< endl;
    cout << "./cliente 8080 127.0.0.1" << endl;
}

bool esUnNumero(const string &str)
{
    for (char const &c : str)
    {
        if (isdigit(c) == 0)
            return false;
    }
    return true;
}

int validateParams(int argc, char *argv[])
{

    if (argc < 2)
    {
        cout << "La cantidad de parametros ingresados no es la suficiente, ante cualquier duda consulta la ayuda ['-h' o '--help']" << endl;
        exit(0);
    }
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
    {
        help();
        exit(1);
    }
    if (!esUnNumero(argv[1]))
    {
        cout << "Parametro no valido, tiene que ser un numero" << endl;
        exit(0);
    }
    if (atoi(argv[1]) <= 8079)
    {
        cout << "El puerto tiene que ser igual o superior al 8080" << endl;
        exit(0);
    }
    if (!esUnNumero(argv[2]))
    {
        cout << "Parametro no valido, la cantidad de jugadores tiene que ser un numero" << endl;
        exit(0);
    }
    if (atoi(argv[2]) < 1)
    {
        cout << "La cantidad de jugadores no puede ser menor a 1, por defecto se asigno 1 jugador" << endl;
        cantJugadores = 1;
    }
    else if (atoi(argv[2]) > 3)
    {
        cout << "La cantidad de jugadores no puede ser mayor a 3, por defecto se asignaron 3 jugadores" << endl;
        cantJugadores = 3;
    }
    else
        cantJugadores = atoi(argv[2]);
    return atoi(argv[1]);
}

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

void jugar(int socket, int j)
{
    char leer[2];
    char escribir[1024] = "conectado al servidor\nesperando a los demas jugadores";
    char op;
    bool cortar = true;
    send(socket, escribir, 1024, 0);
    sleep(1);
    sem_wait(&semInicio);
    strcpy(escribir, "incio el juego, espera tu turno");
    send(socket, escribir, 1024, 0);
    sleep(1);
    while (cortar)
    {
        if (turno == j)
        {
            if (termino)
            {
                send(socket, final().c_str(), 1024, 0);
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
                send(socket, escribir, 1024, 0);
                read(socket, leer, 2);
                ultLetraIng = leer[0];
                // cout << "el jugador ingreso: " << leer[0] << endl;
                int res = ahorcado(ultLetraIng[0], j);
                strcpy(escribir, encotrada.c_str());
                switch (res)
                {
                case 1:
                    // strcat(escribir, "\nacertaste una letra");
                    resMsj = mensaje("acertada");
                    send(socket, resMsj.c_str(), 1024, 0);
                    break;
                case 2:
                    // strcat(escribir, "\nperdiste");
                    resMsj = mensaje("errada");
                    send(socket, resMsj.c_str(), 1024, 0);
                    termino = true;
                    break;
                case 0:
                    // strcat(escribir, "\nganaste");
                    resMsj = mensaje("acertada");
                    send(socket, resMsj.c_str(), 1024, 0);
                    termino = true;
                    break;
                case 3:
                    // strcat(escribir, "\nla letra ingresada no es valida");
                    resMsj = mensaje("errada");
                    send(socket, resMsj.c_str(), 1024, 0);
                    break;
                }
                turno = siguiente(j);
                system("clear");
                cout << resMsj << endl;
                for (int i = 0; i < cantJugadores - 1; i++)
                {
                    sem_post(&semLeer);
                }
                sleep(1);
                letraUsadas = letraUsadas + ultLetraIng;
            }
        }
        else
        {
            if (!termino)
            {
                strcpy(escribir, "ES");
                strcat(escribir, letraUsadas.c_str());
                send(socket, escribir, 1024, 0);
                sem_wait(&semLeer);
                send(socket, resMsj.c_str(), 1024, 0);
                sleep(1);
            }
            else
            {
                cout << final() << endl;
                cortar = false;
                send(socket, final().c_str(), 1024, 0);
                system("clear");
                cout << final() << endl;
            }
        }
    }
    sem_post(&semFin);
    close(socket);
}

int main(int argc, char *argv[])
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    sem_init(&semInicio, 0, 0);
    sem_init(&semLeer, 0, 0);
    sem_init(&semFin, 0, 0);

    system("clear");

    int port = validateParams(argc, argv);

    cout << "servidor de juego iniciado" << endl;
    cout << "en el puerto: " << port << endl;

    palabra=palabraRandom("./palabras.txt");
    faltan=palabra;
    cantLetras=palabra.length();

    cout << "la palabra a adivinar va a ser: " << palabra << endl;


    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < cantJugadores; i++)
    {
        int jugador;
        mutexJugador[i].lock();
        jugador = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (jugador < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        cout << "se conecto el jugador " << i + 1 << endl;
        thread(jugar, jugador, i).detach();
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