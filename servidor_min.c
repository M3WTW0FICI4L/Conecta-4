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
#define ORDRE_FI 9

int main() {
    int s; /* Para trabajar con el socket */
    struct sockaddr_in serv_adr, client_adr;
    char buffer[MIDA_BUFFER];
    socklen_t mida;

    int n;
    int ordre;
    int columna, fila;
    int fichas[COLUMNES];
    int contadorVictoria;
    int j2;

    char tablero[FILES][COLUMNES];

    bool jugada = false;
    bool fi = false;
    bool ganador = false;

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
            columna = 0;
            while (columna < COLUMNES) {
                tablero[fila][columna] = '_';
                fichas[columna] = 0;
                columna++;
            }
            fila = fila - 1;
        }
        columna = 0;
        while (columna < COLUMNES) {
                tablero[FILES][columna] = columna + '0';
                columna++;
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
        sscanf(buffer, "%d", &ordre);
        printf("Paquete recibido %s (columna %d)\n", buffer, ordre);

        //Comprobamos el estado del tablero        
        columna = 0;
        while (fichas[columna] >= FILES) {
            columna++;
            if (columna >= COLUMNES - 1) {
                snprintf(buffer, MIDA_BUFFER, "-TABLERO LLENO-\n");
                sendto(s, buffer, strlen(buffer) + 1, 0, (struct sockaddr *)&client_adr, mida);
                ganador = false;
                fi = true;
            }
        }
        for (fila = 0; fila < FILES; fila++) {
            contadorVictoria = 1;
            for (columna = 0; columna < COLUMNES - 1; columna++) {
                if ((tablero[fila][columna] == tablero[fila][columna+1]) && (tablero[fila][columna] == 'X' || tablero[fila][columna] == 'O')) {
                    contadorVictoria++;
                    if (contadorVictoria >= 4) {
                        fi = true;
                        ganador = true;
                    }
                }
            }
        }
        for (columna = 0; columna < COLUMNES; columna++) {
            contadorVictoria = 1;
            for (fila = 0; fila < FILES - 1; fila++) {
                if ((tablero[fila][columna] == tablero[fila + 1][columna]) && (tablero[fila][columna] == 'X' || tablero[fila][columna] == 'O')) {
                    contadorVictoria++;
                    if (contadorVictoria >= 4) {
                        fi = true;
                        ganador = true;
                    }
                }
            }
        }
        columna = ordre;
        
        //Si no hay 4 en ralla ni el tablero esta lleno, procedemos a las jugadas
        if (fi == false) {
            // Jugador 1
            if (columna < 0 || columna > COLUMNES - 1) {
                snprintf(buffer, MIDA_BUFFER, "Introduce un número entre 0 y %d\n", COLUMNES - 1);
                sendto(s, buffer, strlen(buffer) + 1, 0, (struct sockaddr *)&client_adr, mida);
                jugada = true;
            }
            else {
                if (fichas[columna] >= FILES) {
                    snprintf(buffer, MIDA_BUFFER, "Esta columna está completa\n");
                    sendto(s, buffer, strlen(buffer) + 1, 0, (struct sockaddr *)&client_adr, mida);
                    jugada = true;
                }
                else {
                    tablero[fichas[columna]][columna] = 'X';
                    fichas[columna]++;
                    jugada = false;
                }
             }

            // Jugador 2
            // Semilla para la generación de números aleatorios basada en el tiempo actual
            srand(time(NULL));

            while (jugada == false) {
                // Genera un número aleatorio entre 0 y 5 (ambos incluidos)
                j2 = rand() % (COLUMNES -1);
                if (j2 < 0 || j2 > COLUMNES - 1) {
                    j2 = rand() % (COLUMNES -1);
                }
                else {
                    if (fichas[j2] >= FILES) {
                        j2 = rand() % (COLUMNES -1);
                    } else {
                        tablero[fichas[j2]][j2] = 'O';
                        fichas[j2]++;
                        jugada = true;
                    }
                }
            }

            // Envía el estado actual del tablero al cliente
            fila = FILES; // Empezamos desde la última fila
            sprintf(buffer, "\n");
            while (fila >= 0) {
                int columna = 0;
                while (columna < COLUMNES) {
                    sprintf(buffer + strlen(buffer), "|%c|", tablero[fila][columna]);
                    columna = columna + 1;
                }
                // Agregamos una nueva línea al final de la fila
                sprintf(buffer + strlen(buffer), "\n"); 
                fila = fila - 1;
            }
            // Enviamos respuesta
            sendto(s, buffer, strlen(buffer) + 1, 0, (struct sockaddr *)&client_adr, mida);

        } else {
            if (ganador = true) {
                snprintf(buffer, MIDA_BUFFER, "--VICTORY--\n");
                sendto(s, buffer, strlen(buffer) + 1, 0, (struct sockaddr *)&client_adr, mida);
            } else {
                snprintf(buffer, MIDA_BUFFER, "-GAME OVER-\n");
                sendto(s, buffer, strlen(buffer) + 1, 0, (struct sockaddr *)&client_adr, mida);
            }

            // Generamos una tabla vacía
            fila = FILES;
            while (fila >= 0) {
                columna = 0;
                while (columna < COLUMNES) {
                    tablero[fila][columna] = '_';
                    fichas[columna] = 0;
                    columna++;
                }
                fila = fila - 1;
            }
            columna = 0;
            while (columna < COLUMNES) {
                tablero[FILES][columna] = columna + '0';
                columna++;
            }
            fi = false;
        }
    }
    /* Cerramos el socket */
    close(s);
    return 0;
}
