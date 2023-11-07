/* Inclusión de archivos .h habituales */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

/* Inclusión de archivos .h para los sockets */
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MIDA_BUFFER 1024
#define FILES 6
#define COLUMNES 6

int main() {
    int s; /* Para trabajar con el socket */
    struct sockaddr_in serv_adr, client_adr;
    char buffer[MIDA_BUFFER];
    socklen_t mida;
    int n;
    int columna, fila;
    int fichas[COLUMNES];
    int j2;
    char tablero[FILES][COLUMNES];
    bool jugada = false;
    bool fi = true;

    columna = 0;
    fila = FILES;

    /* Queremos un socket de Internet y no orientado a la conexión */
    s = socket(AF_INET, SOCK_DGRAM, 0);

    /* Configuramos los datos del socket */
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = INADDR_ANY;
    serv_adr.sin_port = htons(44444);

    /* Enlazamos el socket */
    n = bind(s, (struct sockaddr *)&serv_adr, sizeof(serv_adr));
    if (n < 0) {
        printf("No se pudo enlazar el socket\n");
    } else {
        /* ¡Servidor operativo! */
        printf("Servidor operativo!\n\n");

        // Generamos una tabla vacía
        while (fila >= 0) {
            int columna = 0;
            while (columna < COLUMNES) {
                tablero[fila][columna] = '_';
				fichas[columna]=0;
                columna = columna + 1;
            }
            fila = fila - 1;
        }
	}

    while (1) {
            // Recibimos solicitud
            mida = sizeof(client_adr);
            n = recvfrom(s, buffer, MIDA_BUFFER, 0, (struct sockaddr *)&client_adr, &mida);
            if (n < 0) {
                perror("Error al recibir datos\n");
                continue;
            }
            sscanf(buffer, "%d", &columna);
            printf("Paquete recibido %s (columna %d)\n", buffer, columna);

            // Jugador 1
            if (columna < 0 || columna >= COLUMNES) {
                snprintf(buffer, MIDA_BUFFER, "Introduce un número entre 0 y %d\n", COLUMNES - 1);
                sendto(s, buffer, strlen(buffer) + 1, 0, (struct sockaddr *)&client_adr, mida);
            } else {
                if (fichas[columna] >= FILES) {
                    snprintf(buffer, MIDA_BUFFER, "Esta columna está completa\n");
                    sendto(s, buffer, strlen(buffer) + 1, 0, (struct sockaddr *)&client_adr, mida);
                } else {
                    tablero[fichas[columna]][columna] = 'X';
                    fichas[columna] = fichas[columna] + 1;
                }
	        }
            
            // Envía el estado actual del tablero al cliente
            fila = FILES;  // Empezamos desde la última fila
            sprintf(buffer, "Jugada Jugador1:\n");
            while (fila >= 0) {
                int columna = 0;
                while (columna < COLUMNES) {
                    // Agregamos el carácter de la casilla al búfer
                    sprintf(buffer + strlen(buffer), "|%c|", tablero[fila][columna]);
                    columna = columna + 1;
                }
                sprintf(buffer + strlen(buffer), "\n");
                fila = fila - 1;
            }

            // Enviamos el estado del tablero al cliente
            sendto(s, buffer, strlen(buffer) + 1, 0, (struct sockaddr *)&client_adr, mida);

            // Jugador 2
            // Semilla para la generación de números aleatorios basada en el tiempo actual
            srand(time(NULL));

            // Genera un número aleatorio entre 0 y 5 (ambos incluidos)
            j2 = rand() % 6;

            while (jugada == false) {
                if (j2 < 0 || j2 >= COLUMNES) {
                    snprintf(buffer, MIDA_BUFFER, "Generación de aleatorio defectuosa\n");
                    j2 = rand() % 6;
                } else {
                    if (fichas[j2] >= FILES) {
                        snprintf(buffer, MIDA_BUFFER, "Esta columna está completa\n");
                        j2 = rand() % 6;
                    } else {
                        tablero[fichas[j2]][j2] = 'O';
                        fichas[j2] = fichas[j2] + 1;
                        jugada = true;
                    }
                }
            }
            // Envía el estado actual del tablero al cliente
            fila = FILES;  // Empezamos desde la última fila
            sprintf(buffer, "Jugada Jugador2:\n");
            while (fila >= 0) {
                int columna = 0;
                while (columna < COLUMNES) {
                    // Agregamos el carácter de la casilla al búfer
                    sprintf(buffer + strlen(buffer), "|%c|", tablero[fila][columna]);
                    columna = columna + 1;
                }
                // Agregamos una nueva línea al final de la fila
                sprintf(buffer + strlen(buffer), "\n");
                fila = fila - 1;
            }
            jugada = false;

            // Enviamos respuesta
            sendto(s, buffer, strlen(buffer) + 1, 1, (struct sockaddr *)&client_adr, mida);
        }
    /* Cerramos el socket */
    close(s);
    return 0;
}