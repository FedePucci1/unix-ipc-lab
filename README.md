# Unix IPC Lab

Colección de ejercicios de Sistemas Operativos sobre procesos, hilos y mecanismos de comunicación POSIX. El material proviene de un trabajo práctico grupal de la UNLaM y se está preparando como laboratorio reproducible.

## Laboratorios

| Directorio | Tema |
| --- | --- |
| `ejercicio1` | Creación y seguimiento de un árbol de procesos con `fork` y `wait`. |
| `ejercicio2` | Procesamiento concurrente con hilos y semáforos POSIX. |
| `ejercicio3` | Cliente-servidor local mediante FIFO y semáforos con nombre. |
| `ejercicio4` | Juego del ahorcado con memoria compartida y sincronización. |
| `ejercicio5` | Juego del ahorcado en red mediante sockets TCP. |

## Requisitos

- Linux o WSL 2.
- GCC/G++ con soporte para C++17.
- GNU Make.
- Bibliotecas POSIX de hilos y tiempo real.

## Compilación

```bash
make
```

Para borrar los ejecutables generados:

```bash
make clean
```

Cada ejercicio conserva además su makefile original mientras avanza la curaduría.

## Autoría

Trabajo académico grupal de Federico Pucci, Gian Luca Simonetti, Agustín Contreras, Cristian Myroniuk y Simón Bentivenga. Se eliminaron números de documento y otros datos personales de la copia destinada a publicación.

## Estado

La compilación completa fue verificada con GCC/G++ bajo WSL. También se corrigió un desbordamiento de buffer heredado en el servidor FIFO. Quedan pendientes pruebas funcionales automatizadas y el endurecimiento del ejercicio de memoria compartida antes de considerarlo una versión estable.

## Licencia

Pendiente de acuerdo entre los integrantes. Sin una licencia explícita, el código se expone únicamente como muestra académica.
